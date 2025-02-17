#include <stdio.h>
#include <stdint.h>

#include <hardware/gpio.h>
#include <pico/stdlib.h>

#include "bmp280_driver.h"

int main() {

    stdio_init_all();

    i2c_init(BMP280_DEFAULT_I2C_PORT, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    bmp280 bmp280_device = { 0 };
    bmp280_i2c_setup();
    bmp280_i2c_calibrate(&bmp280_device);

    for (;;) {
        bmp280_i2c_read_temperature(&bmp280_device);
        bmp280_i2c_read_pressure(&bmp280_device);
        printf("bmp280 temperature: %f\n", bmp280_device.temperature);
        printf("bmp280 pressure: %f\n", bmp280_device.pressure);
        sleep_ms(1000);
    }

    return 0; 
}