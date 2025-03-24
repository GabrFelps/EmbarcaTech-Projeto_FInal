from flask import Flask, request, jsonify
import requests
import os
import time

app = Flask(__name__)

# Lista global para armazenar os resultados do jogo (ranking)
game_results = []

# Configurações do Telegram (substitua com seu token e chat_id reais, se necessário)
TELEGRAM_BOT_TOKEN = "7587185152:AAFvXujg1NalwiW7gzZJHRBUKJ3w3moenEE"
TELEGRAM_CHAT_ID = "6067913784"

def send_telegram_notification(message):
    """
    Envia a notificação para o Telegram e retorna a resposta da API.
    """
    url = f"https://api.telegram.org/bot{TELEGRAM_BOT_TOKEN}/sendMessage"
    payload = {
        "chat_id": TELEGRAM_CHAT_ID,
        "text": message,
        "parse_mode": "HTML"  # Permite usar tags HTML na mensagem
    }
    for attempt in range(3):  # Tenta até 3 vezes enviar a notificação
        try:
            response = requests.post(url, json=payload)
            if response.status_code == 200:
                return response.json()  # Retorna a resposta da API do Telegram
            print(f"Tentativa {attempt + 1} falhou com código: {response.status_code}")
        except Exception as e:
            print(f"Erro na tentativa {attempt + 1} ao enviar mensagem: {e}")
        time.sleep(2)  # Aguarda 2 segundos antes de tentar novamente
    return {"error": "Falha em todas as tentativas"}

@app.route('/')
def index():
    return "Servidor rodando! Use o endpoint /game_result para enviar e consultar resultados do jogo."

@app.route('/game_result', methods=['GET', 'POST'])
def game_result():
    global game_results

    if request.method == 'POST':
        data = request.get_json()
        # Verifica se todos os campos esperados estão presentes
        required_fields = ['round', 'expectedRed', 'userRed', 'expectedBlue', 'userBlue', 'success']
        if not data or not all(field in data for field in required_fields):
            return jsonify({"error": "Dados incompletos. Campos requeridos: round, expectedRed, userRed, expectedBlue, userBlue, success."}), 400

        round_number = data['round']
        expectedRed = data['expectedRed']
        userRed = data['userRed']
        expectedBlue = data['expectedBlue']
        userBlue = data['userBlue']
        success = data['success']

        # Monta a mensagem para notificação no Telegram
        if success:
            message = (
                f"✅ <b>Rodada {round_number} - Acertou!</b>\n"
                f"Vermelho: {userRed} (esperado: {expectedRed})\n"
                f"Azul: {userBlue} (esperado: {expectedBlue})"
            )
        else:
            message = (
                f"❌ <b>Rodada {round_number} - Errou!</b>\n"
                f"Vermelho: {userRed} (esperado: {expectedRed})\n"
                f"Azul: {userBlue} (esperado: {expectedBlue})\n"
                f"Você perdeu! Qual nome você deseja usar para salvar seu score?"
            )
            # Perguntar o nome para salvar o score
            telegram_response = send_telegram_notification(message)

            # Salva o resultado com a fase alcançada
            player_name = "Player"  # Default, depois vai ser substituído pelo nome do usuário
            game_results.append({
                'name': player_name,
                'score': round_number,
                'success': success
            })

            # Exemplo de uma estrutura para salvar o nome após o jogador responder
            # Aqui você precisará de algum mecanismo para capturar o nome via Telegram, por exemplo
            # com um comando adicional no bot

            # Ordena os resultados por score (fase alcançada)
            game_results = sorted(game_results, key=lambda x: x['score'], reverse=True)

        # Envia a mensagem para o Telegram
        telegram_response = send_telegram_notification(message)
        print("Mensagem enviada ao Telegram:", message)
        print("Resposta do Telegram:", telegram_response)

        # Armazena o resultado recebido
        game_results.append(data)

        return jsonify({
            "status": "success",
            "game_result": data,
            "telegram_response": telegram_response
        }), 200

    elif request.method == 'GET':
        # Retorna o histórico dos resultados do jogo
        return jsonify({"game_results": game_results, "count": len(game_results)}), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
