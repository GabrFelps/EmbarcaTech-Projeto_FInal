#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"       // Biblioteca PWM para os buzzers
#include "ws2812b_animation.h"  // Funções para controle da matriz de LEDs

// Inclusões para Wi‑Fi e LWIP
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/dhcp.h"
#include "lwip/timeouts.h"

// Definições para o display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C  // Endereço I2C do display
#define I2C_SDA 14           // Pino SDA do display
#define I2C_SCL 15           // Pino SCL do display

// Definições dos pinos e componentes
#define BUZZER_A_PIN 21      // Pino do Buzzer A
#define BUZZER_B_PIN 23      // Pino do Buzzer B
#define BUTTON_A_PIN 5       // Botão A (contagem dos LEDs vermelhos)
#define BUTTON_B_PIN 6       // Botão B (contagem dos LEDs azuis)
#define NUM_LEDS 25          // Número total de LEDs na matriz (5x5)
#define LED_PIN 7            // Pino de controle dos LEDs
#define BLUE_LED_PIN 12      // Pino do LED azul (para sinalização, se necessário)

// Tempos iniciais (em milissegundos)
#define INITIAL_PATTERN_TIME 3000
#define INITIAL_RESPONSE_TIME 8000

// Definições de cores (ordem GRB; para vermelho: G=0, R=255, B=0)
#ifndef GRB_BLACK
#define GRB_BLACK 0x00000000
#endif

#ifndef GRB_RED
#define GRB_RED 0x0000FF00
#endif

#ifndef GRB_BLUE
#define GRB_BLUE 0x000000FF
#endif

// Definições Wi‑Fi e servidor
#define WIFI_SSID "JR-2.4G"
#define WIFI_PASSWORD "08642210"
#define API_HOST "192.168.18.143"
#define API_PORT 5000
#define API_PATH "/game_result"

// Variáveis globais para os slices do PWM (buzzers)
uint slice_num_a;
uint slice_num_b;

// Instância do Display OLED
ssd1306_t display;

// Variáveis para escalabilidade da dificuldade
int current_led_count = 5;  // Inicia com 5 LEDs ativos
int roundNumber = 1;        // Número da rodada, usado na equação de tempo

// Prototipação da função para enviar resultados do jogo
bool send_game_result(int round, int expectedRed, int userRed, int expectedBlue, int userBlue, bool success);

// ––– Funções Wi‑Fi e TCP –––

// Função para conectar ao Wi‑Fi e exibir status no OLED, com feedback de debug
void connect_wifi() {
    // DEBUG: Iniciando módulo Wi‑Fi via serial
    printf("DEBUG: Inicializando módulo Wi‑Fi...\n");

    if (cyw43_arch_init()) {
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, 1, "Falha ao iniciar WiFi.");
        ssd1306_show(&display);
        // DEBUG: Falha na inicialização do Wi‑Fi
        printf("DEBUG: Falha na inicializacao do WiFi.\n");
        sleep_ms(3000);
        return;
    }
    // DEBUG: Ativando modo STA
    printf("DEBUG: Habilitando modo STA...\n");
    cyw43_arch_enable_sta_mode();

    // Tenta conectar ao Wi‑Fi com timeout de 30 segundos
    // DEBUG: Tentando conectar na rede: WIFI_SSID
    printf("DEBUG: Conectando na rede Wi‑Fi: %s\n", WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, 1, "Falha ao conectar WiFi.");
        ssd1306_show(&display);
        // DEBUG: Falha na conexão Wi‑Fi
        printf("DEBUG: Falha ao conectar WiFi.\n");
        sleep_ms(3000);
    } else {
        ssd1306_clear(&display);
        int x_centered = (SCREEN_WIDTH - (strlen("Conectado ao WiFi") * 6)) / 2;
        ssd1306_draw_string(&display, x_centered, 16, 1, "Conectado ao WiFi");
        // DEBUG: Conexão Wi‑Fi estabelecida
        printf("DEBUG: Conectado ao Wi‑Fi.\n");

        // Exibe IP obtido via DHCP
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
        // DEBUG: IP exibido via OLED e também via serial
        printf("DEBUG: IP atribuido: %s\n", bufferWifi);
        sleep_ms(5000);
    }
}

// Estrutura para estado do cliente HTTP (para envio via TCP)
typedef struct {
    struct tcp_pcb *pcb;
    bool complete;
    char request[512];
    char response[512];
    size_t response_len;
    ip_addr_t remote_addr;
} HTTP_CLIENT_T;

// Callback para receber dados do servidor
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    HTTP_CLIENT_T *state = (HTTP_CLIENT_T *)arg;
    if (!p) {
        state->complete = true;
        return ERR_OK;
    }
    if (p->tot_len > 0) {
        size_t to_copy = (p->tot_len < sizeof(state->response) - state->response_len - 1) 
            ? p->tot_len : sizeof(state->response) - state->response_len - 1;
        pbuf_copy_partial(p, state->response + state->response_len, to_copy, 0);
        state->response_len += to_copy;
        state->response[state->response_len] = '\0';
    }
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}

// Callback ao conectar-se com o servidor
static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        printf("DEBUG: Erro ao conectar: %d\n", err);
        return err;
    }
    HTTP_CLIENT_T *state = (HTTP_CLIENT_T *)arg;
    err = tcp_write(tpcb, state->request, strlen(state->request), TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        printf("DEBUG: Erro ao enviar requisicao: %d\n", err);
        return err;
    }
    err = tcp_output(tpcb);
    return err;
}

// Função para enviar os resultados do jogo para o servidor via HTTP POST
bool send_game_result(int round, int expectedRed, int userRed, int expectedBlue, int userBlue, bool success) {
    HTTP_CLIENT_T *state = (HTTP_CLIENT_T *)calloc(1, sizeof(HTTP_CLIENT_T));
    if (!state) {
        printf("DEBUG: Erro ao alocar memoria\n");
        return false;
    }
    char json_data[128];
    snprintf(json_data, sizeof(json_data), "{\"round\": %d, \"expectedRed\": %d, \"userRed\": %d, \"expectedBlue\": %d, \"userBlue\": %d, \"success\": %s}",
             round, expectedRed, userRed, expectedBlue, userBlue, success ? "true" : "false");

    // Monta a requisicao HTTP
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

    if (!ipaddr_aton(API_HOST, &state->remote_addr)) {
        printf("DEBUG: Erro ao resolver IP\n");
        free(state);
        return false;
    }
    state->pcb = tcp_new();
    if (!state->pcb) {
        printf("DEBUG: Erro ao criar PCB TCP\n");
        free(state);
        return false;
    }
    tcp_arg(state->pcb, state);
    tcp_recv(state->pcb, tcp_client_recv);
    err_t err = tcp_connect(state->pcb, &state->remote_addr, API_PORT, tcp_client_connected);
    if (err != ERR_OK) {
        printf("DEBUG: Erro ao conectar ao servidor: %d\n", err);
        free(state);
        return false;
    }
    sleep_ms(5000); // Aguarda a resposta
    printf("DEBUG: Resposta da API: %s\n", state->response);
    free(state);
    return true;
}

// ––– Fim das funções Wi‑Fi e TCP –––

// ––– Funções do Jogo –––

// Exibe uma mensagem centralizada no OLED
void display_message(const char *line1, const char *line2) {
    ssd1306_clear(&display);
    int16_t x1 = (SCREEN_WIDTH - strlen(line1) * 6) / 2;
    int16_t x2 = (SCREEN_WIDTH - strlen(line2) * 6) / 2;
    ssd1306_draw_string(&display, x1, 20, 1, line1);
    ssd1306_draw_string(&display, x2, 40, 1, line2);
    ssd1306_show(&display);
    sleep_ms(1500);
}

// Inicializa os botões com resistor de pull-up interno
void init_buttons() {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
}

// Gera um padrão aleatório para os primeiros 'count' LEDs e atualiza os contadores de cores
void generate_led_pattern(uint32_t pattern[NUM_LEDS], int count, int *expectedRed, int *expectedBlue) {
    *expectedRed = 0;
    *expectedBlue = 0;
    for (int i = 0; i < count; i++) {
        int r = rand() % 2; // 0 ou 1
        if (r == 0) {
            pattern[i] = GRB_RED;
            (*expectedRed)++;
        } else {
            pattern[i] = GRB_BLUE;
            (*expectedBlue)++;
        }
    }
    // Preenche os LEDs restantes com preto
    for (int i = count; i < NUM_LEDS; i++) {
        pattern[i] = GRB_BLACK;
    }
}

// Exibe o padrão na matriz de LEDs
void show_led_pattern(uint32_t pattern[NUM_LEDS]) {
    for (int i = 0; i < NUM_LEDS; i++) {
        ws2812b_fill(i, i+1, pattern[i]);
    }
    ws2812b_render();
}

// Toca um beep breve no buzzer indicado (feedback sonoro)
void buzzer_beep(uint slice_num, int duration_ms) {
    pwm_set_enabled(slice_num, true);
    sleep_ms(duration_ms);
    pwm_set_enabled(slice_num, false);
}

// ––– Fim das funções do jogo –––

int main() {
    stdio_init_all();
    // DEBUG: Iniciando I2C para OLED
    printf("DEBUG: Inicializando I2C para display OLED...\n");
    
    // Inicializa I2C para o display OLED
    i2c_init(i2c1, 400 * 1000); // 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED
    if (!ssd1306_init(&display, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_ADDRESS, i2c1)) {
        printf("DEBUG: Falha ao inicializar o display SSD1306\n");
        while (1) { tight_loop_contents(); }
    } else {
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, 1, "Iniciando Jogo...");
        ssd1306_show(&display);
        sleep_ms(150);
    }

    // DEBUG: Inicializando matriz de LEDs
    printf("DEBUG: Inicializando matriz de LEDs (WS2812B)...\n");
    ws2812b_set_global_dimming(7);
    ws2812b_init(pio0, LED_PIN, NUM_LEDS);

    // Inicializa o PWM para os buzzers
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

    init_buttons();
    srand(to_ms_since_boot(get_absolute_time()));

    // Conecta ao Wi‑Fi com feedback de debug
    printf("DEBUG: Chamando função connect_wifi()...\n");
    connect_wifi();

    // Animação de "início"
    int curtain_position = SCREEN_HEIGHT;
    while (curtain_position >= 0) {
        ssd1306_clear(&display);
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            for (int y = 0; y < curtain_position; y++) {
                ssd1306_draw_pixel(&display, x, y);
            }
        }
        ssd1306_show(&display);
        curtain_position -= 2;
        sleep_ms(50);
    }
    // DEBUG: Animação de início concluída
    printf("DEBUG: Animação de inicio concluida.\n");

    // Loop principal do jogo
    while (true) {
        // Calcula os tempos de exibição e de resposta com base na rodada
        int patternDisplayTime = (int)(INITIAL_PATTERN_TIME * pow(1.1, roundNumber));
        int responseTime = (int)(INITIAL_RESPONSE_TIME * pow(1.01, roundNumber));

        uint32_t pattern[NUM_LEDS];
        int expectedRed = 0, expectedBlue = 0;
        generate_led_pattern(pattern, current_led_count, &expectedRed, &expectedBlue);

        // Exibe o padrão na matriz de LEDs
        show_led_pattern(pattern);

        // Exibe instruções iniciais no OLED
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

        // Tempo de visualização do padrão
        sleep_ms(patternDisplayTime);

        // Apaga a matriz para não dar dicas
        ws2812b_fill_all(GRB_BLACK);
        ws2812b_render();

        // Coleta as respostas do jogador dentro do tempo de resposta
        int userRedPresses = 0;
        int userBluePresses = 0;
        uint64_t startTime = to_ms_since_boot(get_absolute_time());
        while (to_ms_since_boot(get_absolute_time()) - startTime < responseTime) {
            if (!gpio_get(BUTTON_A_PIN)) {
                ssd1306_clear(&display);
                userRedPresses++;
                char countMsgv[32];
                char countMsgb[32];
                sprintf(countMsgv, "Vermelho: %d", userRedPresses);
                ssd1306_draw_string(&display, 0, 0, 1, countMsgv);
                sprintf(countMsgb, "Azul: %d", userBluePresses);
                ssd1306_draw_string(&display, 0, 16, 1, countMsgb);
                ssd1306_show(&display);
                sleep_ms(200);  // Debounce
            }
            if (!gpio_get(BUTTON_B_PIN)) {
                ssd1306_clear(&display);
                userBluePresses++;
                char countMsgv[32];
                char countMsgb[32];
                sprintf(countMsgv, "Vermelho: %d", userRedPresses);
                ssd1306_draw_string(&display, 0, 0, 1, countMsgv);
                sprintf(countMsgb, "Azul: %d", userBluePresses);
                ssd1306_draw_string(&display, 0, 16, 1, countMsgb);
                ssd1306_show(&display);
                sleep_ms(200);
            }
        }

        // Verifica a resposta do jogador e toca feedback sonoro
        ssd1306_clear(&display);
        char result[32];
        char result2[32];
        bool success = false;
        if (userRedPresses == expectedRed && userBluePresses == expectedBlue) {
            sprintf(result, "Acertou!");
            buzzer_beep(slice_num_a, 100);
            buzzer_beep(slice_num_b, 100);
            success = true;
            // Envia o resultado da rodada para o servidor via HTTP POST
            send_game_result(roundNumber, expectedRed, userRedPresses, expectedBlue, userBluePresses, success);
            // Se acertou, aumenta a dificuldade
            roundNumber++;
            if (current_led_count < NUM_LEDS) {
                current_led_count++;
            }
        } else {
            sprintf(result, "Voce errou!");
            sprintf(result2, "Fase: %d", roundNumber);
            buzzer_beep(slice_num_a, 300);
            buzzer_beep(slice_num_b, 300);
            // Reinicia dificuldade
            // Envia o resultado da rodada para o servidor via HTTP POST
            send_game_result(roundNumber, expectedRed, userRedPresses, expectedBlue, userBluePresses, success);
            roundNumber = 1;
            current_led_count = 5;
        }
        ssd1306_draw_string(&display, 0, 0, 1, result);
        ssd1306_draw_string(&display, 0, 16, 1, result2);
        char details[32];
        sprintf(details, "R:%d vs %d, B:%d vs %d", expectedRed, userRedPresses, expectedBlue, userBluePresses);
        ssd1306_draw_string(&display, 0, 32, 1, details);
        ssd1306_show(&display);
        sleep_ms(3000);

        ssd1306_clear(&display);
        ssd1306_show(&display);
    }

    return 0;
}
