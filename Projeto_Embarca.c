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

// Tempo para o jogador responder (em milissegundos)
#define TIME_LIMIT_GAME 8000  // Aumentado para 8 segundos

// Definições de cores (ordem GRB)
// Para WS2812, o formato é GRB; para vermelho, G=0, R=255, B=0 → 0x0000FF00
#ifndef GRB_BLACK
#define GRB_BLACK 0x00000000
#endif

#ifndef GRB_RED
#define GRB_RED 0x0000FF00
#endif

#ifndef GRB_BLUE
#define GRB_BLUE 0x000000FF
#endif

// Variáveis globais para os slices do PWM (buzzers)
uint slice_num_a;
uint slice_num_b;

// Instância do Display OLED
ssd1306_t display;

// Variável para escalabilidade da dificuldade
int current_led_count = 5; // Inicia com 5 LEDs ativos

// Função para exibir uma mensagem centralizada no OLED
void display_message(const char *line1, const char *line2) {
    ssd1306_clear(&display);
    int16_t x1 = (SCREEN_WIDTH - strlen(line1) * 6) / 2;
    int16_t x2 = (SCREEN_WIDTH - strlen(line2) * 6) / 2;
    ssd1306_draw_string(&display, x1, 20, 1, line1);
    ssd1306_draw_string(&display, x2, 40, 1, line2);
    ssd1306_show(&display);
    sleep_ms(1500);
}

// Inicializa os botões com pull-up interno
void init_buttons() {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
}

// Gera um padrão aleatório para os primeiros 'count' LEDs e
// define os demais como apagados; atualiza os contadores de cores.
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
    // Preenche o restante com preto
    for (int i = count; i < NUM_LEDS; i++) {
        pattern[i] = GRB_BLACK;
    }
}

// Exibe o padrão na matriz atualizando TODOS os LEDs
void show_led_pattern(uint32_t pattern[NUM_LEDS]) {
    for (int i = 0; i < NUM_LEDS; i++) {
        // Atualiza cada LED conforme o valor armazenado
        ws2812b_fill(i, i+1, pattern[i]);
    }
    ws2812b_render();
}

// Feedback sonoro: toca um beep breve no buzzer indicado
void buzzer_beep(uint slice_num, int duration_ms) {
    pwm_set_enabled(slice_num, true);
    sleep_ms(duration_ms);
    pwm_set_enabled(slice_num, false);
}

int main() {
    stdio_init_all();

    // Inicializa I2C para o display OLED
    i2c_init(i2c1, 400 * 1000); // 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED
    if (!ssd1306_init(&display, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_ADDRESS, i2c1)) {
        printf("Falha ao inicializar o display SSD1306\n");
        while (1) { tight_loop_contents(); }
    } else {
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, 1, "Iniciando Jogo...");
        ssd1306_show(&display);
        sleep_ms(1500);
    }

    // Inicializa a matriz de LEDs (WS2812B)
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

    // Inicializa os botões
    init_buttons();

    // Semente para números aleatórios
    srand(to_ms_since_boot(get_absolute_time()));

    // Animação de "início" (exemplo: cortina abrindo)
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

    // Loop principal do jogo "Led Reflex"
    while (true) {
        uint32_t pattern[NUM_LEDS];
        int expectedRed = 0, expectedBlue = 0;
        generate_led_pattern(pattern, current_led_count, &expectedRed, &expectedBlue);

        // Exibe o padrão na matriz (todos os 25 LEDs são atualizados)
        show_led_pattern(pattern);

        // Exibe instruções no display OLED
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, 1, "Conta as cores!");
        char instr[32];
        sprintf(instr, "Vermelho: ?  Azul: ?");
        ssd1306_draw_string(&display, 0, 16, 1, instr);
        ssd1306_show(&display);

        // Aguarda 3 segundos para que o jogador observe o padrão
        sleep_ms(3000);

        // Apaga a matriz de LEDs para não dar dicas visuais durante a contagem
        ws2812b_fill_all(GRB_BLACK);
        ws2812b_render();

        // Coleta as respostas do jogador dentro do tempo limite
        int userRedPresses = 0;
        int userBluePresses = 0;
        uint64_t startTime = to_ms_since_boot(get_absolute_time());
        while (to_ms_since_boot(get_absolute_time()) - startTime < TIME_LIMIT_GAME) {
            if (!gpio_get(BUTTON_A_PIN)) {
                userRedPresses++;
                printf("Botao A pressionado, count=%d\n", userRedPresses);
                sleep_ms(200);  // Debounce
            }
            if (!gpio_get(BUTTON_B_PIN)) {
                userBluePresses++;
                printf("Botao B pressionado, count=%d\n", userBluePresses);
                sleep_ms(200);  // Debounce
            }
        }

        // Compara a resposta do jogador com os valores esperados
        ssd1306_clear(&display);
        char result[32];
        if (userRedPresses == expectedRed && userBluePresses == expectedBlue) {
            sprintf(result, "Acertou!");
            buzzer_beep(slice_num_a, 100);
            buzzer_beep(slice_num_b, 100);
        } else {
            sprintf(result, "Errou!");
            buzzer_beep(slice_num_a, 300);
            buzzer_beep(slice_num_b, 300);
        }
        ssd1306_draw_string(&display, 0, 0, 1, result);
        char details[32];
        sprintf(details, "R:%d vs %d, B:%d vs %d", expectedRed, userRedPresses, expectedBlue, userBluePresses);
        ssd1306_draw_string(&display, 0, 16, 1, details);
        ssd1306_show(&display);

        // Exibe o resultado por 3 segundos
        sleep_ms(3000);
        ssd1306_clear(&display);
        ssd1306_show(&display);

        // Escalonamento: aumenta o número de LEDs ativos a cada rodada até preencher a matriz
        if (current_led_count < NUM_LEDS) {
            current_led_count++;
        }
    }

    return 0;
}