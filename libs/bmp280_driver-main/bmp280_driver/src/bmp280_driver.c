#include "bmp280_driver.h"

#include <stdlib.h>
#include <pico/stdlib.h>

void bmp280_i2c_write_reg(const uint8_t reg, const uint32_t size, uint8_t* src) {
    uint8_t* buff = (uint8_t*)calloc(size + 1, 1);
    buff[0] = reg;
    for (uint32_t i = 0; i < size; i++) {
        buff[i+1] = src[i];
    }
    i2c_write_blocking(BMP280_DEFAULT_I2C_PORT, BMP280_I2C_ADDRESS, buff, size+1, 0);
    free(buff);
}

void bmp280_i2c_read_reg(const uint8_t reg, const uint32_t size, uint8_t* dst) {
    uint8_t _reg = reg;
    i2c_write_blocking(BMP280_DEFAULT_I2C_PORT, BMP280_I2C_ADDRESS, &reg, 1, 1);
    i2c_read_blocking(BMP280_DEFAULT_I2C_PORT, BMP280_I2C_ADDRESS, dst, size, 0);
}

/*
    BMP280
*/

int bmp280_i2c_setup() {

    sleep_ms(20);

    uint8_t chip_ID;
    bmp280_i2c_read_reg(BMP280_CHIP_ID_REG, 1, &chip_ID);   
    if (chip_ID != BMP280_CHIP_ID) {
        return -1;
    }

    //reset registers
    uint8_t reset_val = BMP280_RESET_VAL;
    bmp280_i2c_write_reg(BMP280_RESET_REG, 1, &reset_val);
    sleep_ms(10);

    // power ctl
    uint8_t ctl_data = (((BMP280_OVERSCAN_X2 << 3) | BMP280_OVERSCAN_X16) << 2) | BMP280_MODE_NORMAL;
    bmp280_i2c_write_reg(BMP280_POWER_CTL_REG, 1, &ctl_data);
    sleep_ms(10);
    return 1;

}

void bmp280_i2c_calibrate(bmp280* device) {
    bmp280_i2c_read_reg(BMP280_CAL_T1_REG, 24, device->coefficients);
}

void bmp280_i2c_read_pressure(bmp280* device) {
    uint8_t data[3];
    bmp280_i2c_read_reg(BMP280_PRESSURE_REG_LOW, 3, data);

    double var1 = 0.0;
    double var2 = 0.0;
    double var3 = 0.0;
    int adc_p = 0;

    adc_p = data[0];
    adc_p <<= 8;
    adc_p |= data[1];
    adc_p <<= 4;
    adc_p |= (data[2] >> 4);

    var1 = (device->temperature * 5120.0 / 2.0) - 64000.0;
    var2 = var1 * var1 * ((int16_t)((device->coefficients[17] << 8) | device->coefficients[16])) / 32768.0;
    var2 = var2 + var1 * ((int16_t)((device->coefficients[15] << 8) | device->coefficients[14])) * 2.0;
    var2 = ( var2 / 4.0 ) + ((int16_t)((device->coefficients[13] << 8) | device->coefficients[12])) * 65536.0;
    var3 = ((int16_t)((device->coefficients[11] << 8) | device->coefficients[10])) * var1 * var1 / 524288.0;
    var1 = ( var3 + ((int16_t)((device->coefficients[9] << 8) | device->coefficients[8])) * var1) / 524288.0;
    var1 = ( 1.0 + var1 / 32768.0) * ((int32_t)((device->coefficients[7] << 8) | device->coefficients[6]));
    device->pressure = 1048576.0 - adc_p;
    device->pressure = ( device->pressure - ( var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((int16_t)((device->coefficients[23] << 8) | device->coefficients[22])) * device->pressure * device->pressure / 2147483648.0;
    var2 = device->pressure * ((int16_t)((device->coefficients[21] << 8) | device->coefficients[20])) / 32768.0;
    device->pressure = device->pressure + ( var1 + var2 + ((int16_t)((device->coefficients[19] << 8) | device->coefficients[18]))) / 16.0;
}

void bmp280_i2c_read_temperature(bmp280* device) {

    uint8_t data[3];
    bmp280_i2c_read_reg(BMP280_TEMPERATURE_REG_LOW, 3, data);
    
    double var1 = 0.0;
    double var2 = 0.0;
    int adc_t = 0.0;


    adc_t = data[0];
    adc_t <<= 8;
    adc_t |= data[1];
    adc_t <<= 4;
    adc_t |= (data[2] >> 4);

    var1 = (adc_t/16384.0 - ((int32_t)((device->coefficients[1] << 8) | device->coefficients[0]))/1024.0) *
           ((int16_t)((device->coefficients[3] << 8) | device->coefficients[2]));

    var2 = (adc_t/131072.0 - ((int16_t)((device->coefficients[1] << 8) | device->coefficients[0]))/8192.0) *
           (adc_t/131072.0 - ((int16_t)((device->coefficients[1] << 8) | device->coefficients[0]))/8192.0) *
           ((int16_t)((device->coefficients[5] << 8) | device->coefficients[4]));

    device->temperature = (var1 + var2) / 5120.0;
}