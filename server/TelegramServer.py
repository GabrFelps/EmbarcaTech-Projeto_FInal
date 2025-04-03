from flask import Flask, request, jsonify
import requests
import os
import time
import json

app = Flask(__name__)

# Arquivos de armazenamento
GAME_RESULTS_FILE = "game_results.json"
RANKING_FILE = "ranking.json"

# Lista global para armazenar os resultados do jogo (todos os resultados)
game_results = []

# Lista global para armazenar os recordes salvos (ranking)
records = []

# Variáveis globais para gerenciar o estado de conversa com o Telegram
pending_record = None  # Armazena o recorde pendente (quando houve erro)
conversation_state = None  # Pode ser "ask_confirmation" ou "ask_name"

# Configurações do Telegram
TELEGRAM_BOT_TOKEN = "7587185152:AAFvXujg1NalwiW7gzZJHRBUKJ3w3moenEE"

def load_data():
    """Carrega os dados salvos nos arquivos JSON, se existirem."""
    global game_results, records
    if os.path.exists(GAME_RESULTS_FILE):
        with open(GAME_RESULTS_FILE, "r", encoding="utf-8") as f:
            try:
                game_results = json.load(f)
            except json.JSONDecodeError:
                game_results = []

    if os.path.exists(RANKING_FILE):
        with open(RANKING_FILE, "r", encoding="utf-8") as f:
            try:
                records = json.load(f)
            except json.JSONDecodeError:
                records = []

def save_data():
    """Salva os resultados e ranking nos arquivos JSON."""
    with open(GAME_RESULTS_FILE, "w", encoding="utf-8") as f:
        json.dump(game_results, f, indent=4, ensure_ascii=False)

    with open(RANKING_FILE, "w", encoding="utf-8") as f:
        json.dump(records, f, indent=4, ensure_ascii=False)

def send_telegram_notification(message):
    """Envia a notificação para o Telegram e retorna a resposta da API."""
    url = f"https://api.telegram.org/bot{TELEGRAM_BOT_TOKEN}/sendMessage"
    payload = {
        "chat_id": 6067913784,
        "text": message,
        "parse_mode": "HTML"
    }
    for attempt in range(3):
        try:
            response = requests.post(url, json=payload)
            if response.status_code == 200:
                return response.json()
            print(f"Tentativa {attempt + 1} falhou com código: {response.status_code}")
        except Exception as e:
            print(f"Erro na tentativa {attempt + 1} ao enviar mensagem: {e}")
        time.sleep(2)
    return {"error": "Falha em todas as tentativas"}

@app.route('/')
def index():
    return "Servidor rodando! Use /game_result para enviar resultados, /rank para consultar os resultados via GET e /webhook para atualizações do Telegram."

@app.route('/game_result', methods=['GET', 'POST'])
def game_result():
    global game_results, pending_record, conversation_state

    if request.method == 'POST':
        data = request.get_json()
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
        message = (
            f"{'✅' if success else '❌'} <b>Rodada {round_number} - {'Acertou!' if success else 'Errou!'}</b>\n"
            f"Vermelho: {userRed} (esperado: {expectedRed})\n"
            f"Azul: {userBlue} (esperado: {expectedBlue})"
        )

        telegram_response = send_telegram_notification(message)

        game_results.append(data)
        save_data()  # Salva os resultados imediatamente

        # Se o jogador errou, inicia o processo para salvar o recorde
        if not success:
            pending_record = {
                "round": round_number,
                "expectedRed": expectedRed,
                "userRed": userRed,
                "expectedBlue": expectedBlue,
                "userBlue": userBlue,
                "timestamp": time.time()
            }
            conversation_state = "ask_confirmation"
            send_telegram_notification("❌ Você errou!\nDeseja salvar o recorde? Responda com <b>sim</b> ou <b>nao</b>.")

        return jsonify({
            "status": "success",
            "game_result": data,
            "telegram_response": telegram_response
        }), 200

    elif request.method == 'GET':
        return jsonify({"game_results": game_results, "count": len(game_results)}), 200

@app.route('/rank', methods=['GET'])
def get_rank():
    """Retorna os recordes salvos, ordenados pela fase (round) de forma decrescente."""
    sorted_records = sorted(records, key=lambda r: r.get("round", 0), reverse=True)
    return jsonify({"records": sorted_records, "count": len(sorted_records)}), 200

@app.route('/webhook', methods=['POST'])
def webhook():
    global pending_record, conversation_state, records

    update = request.get_json()
    if not update or "message" not in update:
        return "ok", 200

    message = update["message"]
    text = message.get("text", "").strip().lower()

    print(f"DEBUG: Recebido text='{text}', conversation_state={conversation_state}")

    if text in ["/showrank", "/rank"]:
        sorted_records = sorted(records, key=lambda r: r.get("round", 0), reverse=True)
        rank_msg = "🏆 <b>Ranking dos Jogadores</b>:\n" if sorted_records else "🏆 <b>Nenhum recorde salvo ainda.</b>"
        for i, rec in enumerate(sorted_records, start=1):
            nome = rec.get("name", "Sem nome")
            fase = rec.get("round", 0)
            rank_msg += f"{i}º - {nome} (Fase: {fase})\n"
        send_telegram_notification(rank_msg)
        return "ok", 200

    if conversation_state == "ask_confirmation":
        if text == "sim":
            send_telegram_notification("Ótimo! Qual o seu nome?")
            conversation_state = "ask_name"
        elif text == "nao":
            send_telegram_notification("Recorde não será salvo.")
            pending_record = None
            conversation_state = None
    elif conversation_state == "ask_name":
        pending_record["name"] = text
        records.append(pending_record)
        save_data()  # Salva os recordes imediatamente
        send_telegram_notification(f"Recorde salvo com sucesso, {text}!")
        pending_record = None
        conversation_state = None

    return "ok", 200

if __name__ == '__main__':
    load_data()  # Carregar os dados antes de iniciar
    app.run(host='0.0.0.0', port=5000, debug=True)
