# LedReflex: Jogo de Reflexo com Matriz de LED's
O LED REFLEX é um jogo interativo desenvolvido com o objetivo de aprimorar a agilidade cognitiva e a coordenação motora do jogador. Utilizando uma matriz de LEDs da BitDogLab, o jogo testa a rapidez e precisão na contagem de LEDs vermelhos e azuis acesos aleatoriamente. Com uma mecânica simples, o jogo envolve o jogador na identificação e contagem dessas cores, interagindo com os botões da placa e respondendo a tempo determinado. Além disso, a dificuldade do jogo aumenta conforme o jogador avança nas rodadas, o que ocorre através de um aumento progressivo no número de LEDs acesos e um ajuste no tempo de resposta, modelado por uma equação exponencial. O feedback é gerado por meio de um display OLED, buzzer para alertas sonoros e um sistema de comunicação via Wi-Fi para interação remota.
## Descrição do Funcionamento
**Exibição das Cores:** A matriz de LEDs WS2812B (5x5) é composta por 25 LEDs endereçáveis, sendo capaz de exibir padrões de cores dinâmicas. Durante cada rodada, LEDs vermelhos e azuis são acesos aleatoriamente. Nos primeiros estágios do jogo, o número de LEDs ativos é pequeno (por exemplo, 5 LEDs), mas conforme o jogador avança, mais LEDs são ativados, tornando o desafio mais complexo.


**Contagem Interna:** O sistema registra a quantidade de LEDs vermelhos e azuis que foram acesos. Esse registro é essencial para a interação com o usuário e a comparação de respostas.

**Interação com o Usuário:** Durante a fase de contagem, o jogador interage utilizando os Botões A e B:


- O **Botão A** é utilizado para registrar a quantidade de LEDs vermelhos.
  
- O **Botão B** é usado para registrar a quantidade de LEDs azuis.

À medida que o jogador interage, o Display OLED exibe a contagem dinâmica de LEDs vermelhos e azuis, atualizando conforme o progresso da rodada.
Verificação da Resposta: Após o tempo de resposta determinado (ajustável de acordo com a rodada), o sistema compara a resposta do jogador com os valores registrados internamente. Caso a resposta seja correta, o jogo emite um feedback sonoro positivo e aumenta a quantidade de LEDs ativos, além de ajustar os tempos de exibição e resposta segundo a equação de progressão:

<p style="text-align:center;"> tempo = tempo_inicial * (1.1 ^ t)</p>

onde t é o número da rodada.
Caso a resposta esteja incorreta, o jogo exibe no display a fase alcançada (quantidade de LEDs ativos) e emite um alerta sonoro de erro. O jogo então reinicia com os parâmetros iniciais, proporcionando uma nova tentativa.
Integração Wi-Fi: Uma melhoria recente foi a integração do módulo Wi-Fi da placa BitDogLab, que permite ao sistema se conectar a um servidor local e enviar os dados do jogo para um Bot do Telegram. Esse bot, criado com o BotFather, recebe informações do jogo e as compartilha com o jogador em tempo real, como feedbacks de progresso e performance. Além disso, um feedback de conexão Wi-Fi é exibido no display OLED, mostrando o IP de conexão da placa, garantindo que o usuário saiba quando o dispositivo está devidamente conectado.


Feedback Visual e Sonoro: Durante todo o processo de jogo, a música e os sons de feedback são emitidos pelo buzzer conectado ao GPIO 21 da BitDogLab. Isso serve para informar ao jogador se ele acertou ou errou a contagem, além de alertar o usuário sobre os eventos do jogo.
