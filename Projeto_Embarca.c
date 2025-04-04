#include <stdio.h>            // Funções padrão de entrada/saída
#include <stdint.h>           // Tipos de dados com tamanho fixo (ex.: uint32_t)
#include <stdlib.h>           // Funções utilitárias, como malloc(), rand(), etc.
#include <string.h>           // Funções para manipulação de strings
#include <math.h>             // Funções matemáticas (ex.: pow())

// Bibliotecas específicas do Raspberry Pi Pico e periféricos
#include "pico/stdlib.h"      // Funções padrão para Raspberry Pi Pico (GPIO, sleep, etc.)
#include "ssd1306.h"          // Driver para o display OLED SSD1306
#include "hardware/i2c.h"     // Controle do barramento I2C
#include "hardware/pwm.h"     // Controle do PWM, utilizado para os buzzers
#include "ws2812b_animation.h"// Funções para controle da matriz de LEDs WS2812B

// Bibliotecas para conexão Wi‑Fi e pilha TCP/IP (LWIP)
#include "pico/cyw43_arch.h"  // Driver do módulo Wi‑Fi do Pico W
#include "lwip/tcp.h"         // Funções para manipulação do protocolo TCP
#include "lwip/dhcp.h"        // Protocolo DHCP para obtenção de IP
#include "lwip/timeouts.h"    // Funções para gerenciamento de timeouts em conexões TCP

// ----- Definições para o Display OLED -----
#define SCREEN_WIDTH 128      // Largura do display OLED
#define SCREEN_HEIGHT 64      // Altura do display OLED
#define SCREEN_ADDRESS 0x3C   // Endereço I2C do display OLED (geralmente 0x3C)
#define I2C_SDA 14            // Pino SDA para comunicação I2C com o display
#define I2C_SCL 15            // Pino SCL para comunicação I2C com o display

// ----- Definições dos pinos e componentes do jogo -----
#define BUZZER_A_PIN 21       // Pino do primeiro buzzer (A)
#define BUZZER_B_PIN 23       // Pino do segundo buzzer (B)
#define BUTTON_A_PIN 5        // Botão A: pode ser usado para iniciar o jogo ou contar LED vermelho
#define BUTTON_B_PIN 6        // Botão B: usado para contar LED azul
#define NUM_LEDS 25           // Número total de LEDs na matriz (por exemplo, 5x5)
#define LED_PIN 7             // Pino de controle dos LEDs WS2812B

// ----- Definições de tempos (em milissegundos) -----
#define INITIAL_PATTERN_TIME 3000  // Tempo para exibição do padrão de LEDs (3 segundos)
#define INITIAL_RESPONSE_TIME 8000 // Tempo que o jogador tem para responder (8 segundos)

// ----- Definições de cores (no formato GRB, comum para LEDs WS2812B) -----
#ifndef GRB_BLACK
#define GRB_BLACK 0x00000000  // Apagado (preto)
#endif

#ifndef GRB_RED
#define GRB_RED 0x0000FF00    // Vermelho: 0x00 (G) | 0xFF (R) | 0x00 (B)
#endif

#ifndef GRB_BLUE
#define GRB_BLUE 0x000000FF   // Azul: 0x00 (G) | 0x00 (R) | 0xFF (B)
#endif

// ----- Definições para Wi‑Fi e dados do servidor (API) -----
#define WIFI_SSID "Felps"          // Nome da rede Wi‑Fi
#define WIFI_PASSWORD "Felps2006@."     // Senha da rede Wi‑Fi
#define API_HOST "192.168.125.63"    // Endereço IP do servidor da API
#define API_PORT 5000                // Porta da API
#define API_PATH "/game_result"      // Rota onde os resultados serão enviados

// ----- Variáveis globais para os slices PWM dos buzzers -----
uint slice_num_a;  // Slice do PWM para o Buzzer A
uint slice_num_b;  // Slice do PWM para o Buzzer B

// ----- Instância do display OLED -----
ssd1306_t display; // Estrutura que representa o display OLED

// ----- Variáveis para escalabilidade do jogo -----
int current_led_count;  // Número de LEDs ativos na rodada atual
int roundNumber;        // Número da rodada (fase do jogo)

// ----- Prototipação da função para enviar os resultados do jogo para a API -----
bool send_game_result(int round, int expectedRed, int userRed, int expectedBlue, int userBlue, bool success);

// ---------------------------------------------------------------
// Funções Wi‑Fi e TCP/IP para conexão com o servidor e envio dos dados
// ---------------------------------------------------------------

// Função para conectar à rede Wi‑Fi e exibir informações no display OLED
void connect_wifi() {
    printf("DEBUG: Inicializando módulo Wi‑Fi...\n");
    if (cyw43_arch_init()) { // Inicializa o módulo Wi‑Fi; retorna não zero se falhar
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, 1, "Falha ao iniciar WiFi.");
        ssd1306_show(&display);
        printf("DEBUG: Falha na inicializacao do WiFi.\n");
        sleep_ms(3000);
        return;
    }
    printf("DEBUG: Habilitando modo STA...\n");
    cyw43_arch_enable_sta_mode(); // Habilita o modo Station (conectar-se a uma rede)
    printf("DEBUG: Conectando na rede Wi‑Fi: %s\n", WIFI_SSID);
    // Tenta conectar à rede Wi‑Fi com timeout de 30 segundos
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, 1, "Falha ao conectar WiFi.");
        ssd1306_show(&display);
        printf("DEBUG: Falha ao conectar WiFi.\n");
        sleep_ms(3000);
    } else {
        ssd1306_clear(&display);
        // Exibe mensagem centralizada no display indicando sucesso na conexão
        int x_centered = (SCREEN_WIDTH - (strlen("Conectado ao WiFi") * 6)) / 2;
        ssd1306_draw_string(&display, x_centered, 16, 1, "Conectado ao WiFi");
        printf("DEBUG: Conectado ao Wi‑Fi.\n");
        // Obtém e exibe o endereço IP
        ip4_addr_t ip = cyw43_state.netif[0].ip_addr;
        uint32_t ip_raw = ip4_addr_get_u32(&ip);
        uint8_t ip_address[4] = {
            (uint8_t)(ip_raw & 0xFF),
            (uint8_t)((ip_raw >> 8) & 0xFF),
            (uint8_t)((ip_raw >> 16) & 0xFF),
            (uint8_t)((ip_raw >> 24) & 0xFF)
        };
        char bufferWifi[50];
        sprintf(bufferWifi, "IP: %d.%d.%d.%d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
        x_centered = (SCREEN_WIDTH - (strlen(bufferWifi) * 6)) / 2;
        ssd1306_draw_string(&display, x_centered, 32, 1, bufferWifi);
        ssd1306_show(&display);
        printf("DEBUG: IP atribuido: %s\n", bufferWifi);
        sleep_ms(5000);
    }
}

// Estrutura para manter o estado da conexão HTTP/TCP
typedef struct {
    struct tcp_pcb *pcb;      // Estrutura PCB do TCP
    bool complete;            // Flag para indicar que a resposta foi completamente recebida
    char request[512];        // Buffer para armazenar a requisição HTTP
    char response[512];       // Buffer para armazenar a resposta HTTP
    size_t response_len;      // Tamanho da resposta recebida
    ip_addr_t remote_addr;    // Endereço IP do servidor remoto
} HTTP_CLIENT_T;

// Função callback para receber dados via TCP
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    HTTP_CLIENT_T *state = (HTTP_CLIENT_T *)arg;
    if (!p) {  // Se não houver pacote, a transmissão está completa
        state->complete = true;
        return ERR_OK;
    }
    if (p->tot_len > 0) {
        // Copia os dados recebidos para o buffer de resposta
        size_t to_copy = (p->tot_len < sizeof(state->response) - state->response_len - 1)
            ? p->tot_len : sizeof(state->response) - state->response_len - 1;
        pbuf_copy_partial(p, state->response + state->response_len, to_copy, 0);
        state->response_len += to_copy;
        state->response[state->response_len] = '\0';
    }
    tcp_recved(tpcb, p->tot_len); // Informa ao TCP que os dados foram processados
    pbuf_free(p);  // Libera o buffer recebido
    return ERR_OK;
}

// Função callback chamada ao conectar com sucesso ao servidor
static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {  // Verifica se houve erro na conexão
        printf("DEBUG: Erro ao conectar: %d\n", err);
        return err;
    }
    HTTP_CLIENT_T *state = (HTTP_CLIENT_T *)arg;
    // Envia a requisição HTTP para o servidor
    err = tcp_write(tpcb, state->request, strlen(state->request), TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        printf("DEBUG: Erro ao enviar requisicao: %d\n", err);
        return err;
    }
    err = tcp_output(tpcb); // Garante que os dados sejam enviados
    return err;
}

// Função que envia os resultados do jogo para a API via requisição HTTP POST
bool send_game_result(int round, int expectedRed, int userRed, int expectedBlue, int userBlue, bool success) {
    // Aloca memória para o estado do cliente HTTP
    HTTP_CLIENT_T *state = (HTTP_CLIENT_T *)calloc(1, sizeof(HTTP_CLIENT_T));
    if (!state) {
        printf("DEBUG: Erro ao alocar memoria\n");
        return false;
    }
    // Cria um JSON com os dados do jogo
    char json_data[128];
    snprintf(json_data, sizeof(json_data),
             "{\"round\": %d, \"expectedRed\": %d, \"userRed\": %d, \"expectedBlue\": %d, \"userBlue\": %d, \"success\": %s}",
             round, expectedRed, userRed, expectedBlue, userBlue, success ? "true" : "false");

    // Prepara a requisição HTTP POST com os dados JSON
    snprintf(state->request, sizeof(state->request),
             "POST %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             API_PATH, API_HOST, (int)strlen(json_data), json_data);

    printf("DEBUG: Enviando JSON para API...\n%s\n", state->request);

    // Resolve o endereço IP do servidor
    if (!ipaddr_aton(API_HOST, &state->remote_addr)) {
        printf("DEBUG: Erro ao resolver IP\n");
        free(state);
        return false;
    }
    // Cria uma nova PCB TCP
    state->pcb = tcp_new();
    if (!state->pcb) {
        printf("DEBUG: Erro ao criar PCB TCP\n");
        free(state);
        return false;
    }
    tcp_arg(state->pcb, state);
    tcp_recv(state->pcb, tcp_client_recv);
    // Conecta ao servidor na porta definida
    err_t err = tcp_connect(state->pcb, &state->remote_addr, API_PORT, tcp_client_connected);
    if (err != ERR_OK) {
        printf("DEBUG: Erro ao conectar ao servidor: %d\n", err);
        free(state);
        return false;
    }
    // Aguarda um tempo para que a resposta seja recebida (ajustável conforme necessidade)
    sleep_ms(5000);
    printf("DEBUG: Resposta da API: %s\n", state->response);
    free(state);
    return true;
}

// ---------------------------------------------------------------
// Funções do jogo: exibição, geração de padrões e lógica
// ---------------------------------------------------------------

// Função para exibir uma mensagem simples no display OLED com duas linhas
void display_message(const char *line1, const char *line2) {
    ssd1306_clear(&display);  // Limpa o display
    // Calcula a posição central para as mensagens
    int16_t x1 = (SCREEN_WIDTH - strlen(line1) * 6) / 2;
    int16_t x2 = (SCREEN_WIDTH - strlen(line2) * 6) / 2;
    ssd1306_draw_string(&display, x1, 20, 1, line1);
    ssd1306_draw_string(&display, x2, 40, 1, line2);
    ssd1306_show(&display);
    sleep_ms(1500);  // Aguarda 1,5 segundos para que a mensagem seja lida
}

// Função que inicializa os botões, configurando-os como entrada com pull-up
void init_buttons() {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
}

// Função que gera um padrão de LEDs com cores aleatórias (vermelho ou azul)
// "count" indica quantos LEDs estarão ativos; os demais serão apagados (preto)
void generate_led_pattern(uint32_t pattern[NUM_LEDS], int count, int *expectedRed, int *expectedBlue) {
    *expectedRed = 0;
    *expectedBlue = 0;
    for (int i = 0; i < count; i++) {
        int r = rand() % 2;  // Gera 0 ou 1 aleatoriamente
        if (r == 0) {
            pattern[i] = GRB_RED;
            (*expectedRed)++;  // Incrementa o contador de LEDs vermelhos esperados
        } else {
            pattern[i] = GRB_BLUE;
            (*expectedBlue)++; // Incrementa o contador de LEDs azuis esperados
        }
    }
    // Para os LEDs que não serão usados, define como apagado (preto)
    for (int i = count; i < NUM_LEDS; i++) {
        pattern[i] = GRB_BLACK;
    }
}

// Função que exibe o padrão de LEDs na matriz WS2812B
void show_led_pattern(uint32_t pattern[NUM_LEDS]) {
    for (int i = 0; i < NUM_LEDS; i++) {
        ws2812b_fill(i, i+1, pattern[i]);  // Atualiza o LED na posição i com a cor definida
    }
    ws2812b_render();  // Renderiza a matriz de LEDs com as cores configuradas
}

// Função para tocar um beep no buzzer usando PWM
// "slice_num" é o número do slice PWM, e "duration_ms" a duração do beep
void buzzer_beep(uint slice_num, int duration_ms) {
    pwm_set_enabled(slice_num, true);  // Habilita o PWM (ativa o buzzer)
    sleep_ms(duration_ms);             // Aguarda a duração definida
    pwm_set_enabled(slice_num, false); // Desliga o PWM (desativa o buzzer)
}

// Função que exibe a tela inicial e aguarda o pressionamento do botão A para iniciar o jogo
void show_initial_menu() {
    ssd1306_clear(&display);  // Limpa o display
    // Calcula a posição central para o título do jogo
    int x_center = (SCREEN_WIDTH - strlen("LedReflex Game") * 6) / 2;
    int x_center2 = (SCREEN_WIDTH - strlen("Aperte A para iniciar") * 6 / 2);
    ssd1306_draw_string(&display, x_center, 20, 1, "LedReflex Game");
    // Exibe instrução para iniciar o jogo
    x_center = (SCREEN_WIDTH - strlen("aperte A para iniciar")) / 2;
    ssd1306_draw_string(&display, x_center2, 40, 1, "press A to start");
    ssd1306_show(&display);
    // Loop que aguarda até o botão A ser pressionado (estado ativo baixo)
    while (gpio_get(BUTTON_A_PIN)) {
        sleep_ms(100);
    }
    sleep_ms(200); // Delay para debounce
}

// ---------------------------------------------------------------
// Função principal: inicialização dos componentes e loop do jogo
// ---------------------------------------------------------------
int main() {
    stdio_init_all();  // Inicializa a comunicação serial (USB, etc.)
    
    // Inicializa o I2C para o display OLED com frequência de 400 kHz
    printf("DEBUG: Inicializando I2C para display OLED...\n");
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED; se falhar, entra em loop infinito
    if (!ssd1306_init(&display, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_ADDRESS, i2c1)) {
        printf("DEBUG: Falha ao inicializar o display SSD1306\n");
        while (1) { tight_loop_contents(); }
    } else {
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, 1, "Iniciando Jogo...");
        ssd1306_show(&display);
        sleep_ms(150);
    }

    // Inicializa a matriz de LEDs WS2812B
    printf("DEBUG: Inicializando matriz de LEDs (WS2812B)...\n");
    ws2812b_set_global_dimming(7);
    ws2812b_init(pio0, LED_PIN, NUM_LEDS);

    // Configuração dos buzzers com PWM
    gpio_set_function(BUZZER_A_PIN, GPIO_FUNC_PWM);
    slice_num_a = pwm_gpio_to_slice_num(BUZZER_A_PIN);
    pwm_set_wrap(slice_num_a, 25000);
    pwm_set_gpio_level(BUZZER_A_PIN, 12500);
    pwm_set_enabled(slice_num_a, false);

    gpio_set_function(BUZZER_B_PIN, GPIO_FUNC_PWM);
    slice_num_b = pwm_gpio_to_slice_num(BUZZER_B_PIN);
    pwm_set_wrap(slice_num_b, 25000);
    pwm_set_gpio_level(BUZZER_B_PIN, 12500);
    pwm_set_enabled(slice_num_b, false);

    // Inicializa os botões
    init_buttons();
    // Inicializa a semente do gerador de números aleatórios com base no tempo de boot
    srand(to_ms_since_boot(get_absolute_time()));

    // Conecta ao Wi‑Fi e exibe informações no display
    printf("DEBUG: Chamando função connect_wifi()...\n");
    connect_wifi();

    // Animação de "cortina" no display OLED para transição
    int curtain_position = SCREEN_HEIGHT;
    while (curtain_position >= 0) {
        ssd1306_clear(&display);
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            for (int y = 0; y < curtain_position; y++) {
                ssd1306_draw_pixel(&display, x, y);  // Preenche os pixels até a posição da cortina
            }
        }
        ssd1306_show(&display);
        curtain_position -= 2;
        sleep_ms(50);
    }
    printf("DEBUG: Animação de inicio concluida.\n");

    // Loop principal do jogo: reinicia o jogo após término de cada partida
    while (true) {
        // Exibe a tela inicial e aguarda o início do jogo
        show_initial_menu();
        // Reinicia os parâmetros do jogo: rodada começa na 1 e 5 LEDs ativos
        roundNumber = 1;
        current_led_count = 5;
        
        // Loop de rodadas do jogo
        while (true) {
            // Calcula o tempo de exibição do padrão e o tempo de resposta com base na rodada atual
            int patternDisplayTime = (int)(INITIAL_PATTERN_TIME * pow(1.1, roundNumber));
            int responseTime = (int)(INITIAL_RESPONSE_TIME * pow(1.01, roundNumber));

            uint32_t pattern[NUM_LEDS];
            int expectedRed = 0, expectedBlue = 0;
            // Gera um padrão com "current_led_count" LEDs ativos e conta os LEDs de cada cor
            generate_led_pattern(pattern, current_led_count, &expectedRed, &expectedBlue);

            // Exibe o padrão de LEDs na matriz
            show_led_pattern(pattern);

            // Exibe instruções no display OLED para o jogador contar as cores
            ssd1306_clear(&display);
            ssd1306_draw_string(&display, 0, 0, 1, "Conte as cores!");
            char instr[32];
            sprintf(instr, "Vermelho: ? | Azul: ?");
            ssd1306_draw_string(&display, 0, 16, 1, instr);
            sprintf(instr, "A p/ Vermelho");
            ssd1306_draw_string(&display, 0, 32, 1, instr);
            sprintf(instr, "B p/ Azul");
            ssd1306_draw_string(&display, 0, 48, 1, instr);
            ssd1306_show(&display);

            // Aguarda o tempo definido para exibir o padrão
            sleep_ms(patternDisplayTime);

            // Após o tempo, apaga os LEDs da matriz
            ws2812b_fill_all(GRB_BLACK);
            ws2812b_render();

            // Variáveis para contar os pressionamentos do usuário
            int userRedPresses = 0;
            int userBluePresses = 0;
            uint64_t startTime = to_ms_since_boot(get_absolute_time());
            // Loop para capturar os pressionamentos dos botões dentro do tempo de resposta
            while (to_ms_since_boot(get_absolute_time()) - startTime < responseTime) {
                if (!gpio_get(BUTTON_A_PIN)) { // Botão A pressionado
                    ssd1306_clear(&display);
                    userRedPresses++;
                    char countMsgv[32], countMsgb[32];
                    sprintf(countMsgv, "Vermelho: %d", userRedPresses);
                    ssd1306_draw_string(&display, 0, 0, 1, countMsgv);
                    sprintf(countMsgb, "Azul: %d", userBluePresses);
                    ssd1306_draw_string(&display, 0, 16, 1, countMsgb);
                    ssd1306_show(&display);
                    sleep_ms(200);  // Debounce e evita múltiplos registros indesejados
                }
                if (!gpio_get(BUTTON_B_PIN)) { // Botão B pressionado
                    ssd1306_clear(&display);
                    userBluePresses++;
                    char countMsgv[32], countMsgb[32];
                    sprintf(countMsgv, "Vermelho: %d", userRedPresses);
                    ssd1306_draw_string(&display, 0, 0, 1, countMsgv);
                    sprintf(countMsgb, "Azul: %d", userBluePresses);
                    ssd1306_draw_string(&display, 0, 16, 1, countMsgb);
                    ssd1306_show(&display);
                    sleep_ms(200);
                }
            }

            // Após o período de resposta, verifica se o jogador acertou a contagem
            ssd1306_clear(&display);
            char result[32] = "";
            char result2[32] = "";
            bool success = false;
            if (userRedPresses == expectedRed && userBluePresses == expectedBlue) {
                // Caso a contagem esteja correta
                sprintf(result, "Acertou!");
                ssd1306_draw_string(&display, 0, 0, 1, result);
                // Emite beep curto em ambos os buzzers
                buzzer_beep(slice_num_a, 100);
                buzzer_beep(slice_num_b, 100);
                success = true;
                send_game_result(roundNumber, expectedRed, userRedPresses, expectedBlue, userBluePresses, success);
                roundNumber++;  // Avança para a próxima rodada
                if (current_led_count < NUM_LEDS) {
                    current_led_count++;  // Aumenta a quantidade de LEDs ativos para aumentar a dificuldade
                }
            } else {
                // Se a contagem estiver incorreta, indica erro
                sprintf(result, "Voce errou!");
                sprintf(result2, "Fase: %d", roundNumber);
                ssd1306_draw_string(&display, 12, 0, 1, result);
                ssd1306_draw_string(&display, 25, 0, 1, result2);
                // Emite beep longo em ambos os buzzers
                buzzer_beep(slice_num_a, 300);
                buzzer_beep(slice_num_b, 300);
                send_game_result(roundNumber, expectedRed, userRedPresses, expectedBlue, userBluePresses, success);
                // Exibe mensagem de erro e fase atingida
                ssd1306_draw_string(&display, 0, 0, 1, result);
                ssd1306_draw_string(&display, 0, 16, 1, result2);
                ssd1306_draw_string(&display, 0, 32, 1, "Encerrando...");
                ssd1306_show(&display);
                sleep_ms(3000);
                // Encerra a partida, retornando à tela inicial
                break;
            }
            
            // Limpa o display entre as rodadas
            ssd1306_clear(&display);
            ssd1306_show(&display);
        }
    }

    return 0;
}
