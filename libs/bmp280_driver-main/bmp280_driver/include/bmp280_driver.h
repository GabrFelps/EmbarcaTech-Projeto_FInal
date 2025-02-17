#ifndef BMP280_DRIVER_H
#define BMP280_DRIVER_H

#include <stdint.h>
#include <hardware/i2c.h>

#define BMP280_DEFAULT_I2C_PORT i2c0

/*
    BMP280
*/

#define BMP280_I2C_ADDRESS 0x77
#define BMP280_CHIP_ID_REG 0xD0
#define BMP280_CHIP_ID 0x58
#define BMP280_POWER_CTL_REG 0xF4
#define BMP280_RESET_REG 0xE0
#define BMP280_RESET_VAL 0xB6
#define BMP280_PRESSURE_REG_LOW 0xF7
#define BMP280_PRESSURE_REG_HIGH 0xF9
#define BMP280_TEMPERATURE_REG_LOW 0xFA
#define BMP280_TEMPERATURE_REG_HIGH 0xFC

#define BMP280_CAL_T1_REG 0x88
#define BMP280_CAL_T2_REG 0x8A
#define BMP280_CAL_T3_REG 0x8C
#define BMP280_CAL_P1_REG 0x8E
#define BMP280_CAL_P2_REG 0x90
#define BMP280_CAL_P3_REG 0x92
#define BMP280_CAL_P4_REG 0x94
#define BMP280_CAL_P5_REG 0x96
#define BMP280_CAL_P6_REG 0x98
#define BMP280_CAL_P7_REG 0x9A
#define BMP280_CAL_P8_REG 0x9C
#define BMP280_CAL_P9_REG 0x9E

#define BMP280_OVERSCAN_DISABLE 0x00
#define BMP280_OVERSCAN_X1      0x01
#define BMP280_OVERSCAN_X2      0x02
#define BMP280_OVERSCAN_X4      0x03
#define BMP280_OVERSCAN_X8      0x04
#define BMP280_OVERSCAN_X16     0x05

#define BMP280_MODE_SLEEP   0x00
#define BMP280_MODE_FORCE   0x01
#define BMP280_MODE_NORMAL  0x03



typedef struct bmp280 {

    uint8_t coefficients[24];

    double pressure;
    double temperature;

} bmp280;



extern void bmp280_i2c_read_reg(const uint8_t reg, const uint32_t size, uint8_t* dst);

extern void bmp280_i2c_write_reg(const uint8_t reg, const uint32_t size, uint8_t* src);



extern int bmp280_i2c_setup();

extern void bmp280_i2c_calibrate(bmp280* device);

extern void bmp280_i2c_read_pressure(bmp280* device);

extern void bmp280_i2c_read_temperature(bmp280* device);



#endif // I2C_H
