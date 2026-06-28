#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#include "main.h"

/* 引脚配置 — 按实际接线修改 */
#define SOFT_I2C_SCL_PORT       GPIOH
#define SOFT_I2C_SCL_PIN        GPIO_PIN_7
#define SOFT_I2C_SDA_PORT       GPIOH
#define SOFT_I2C_SDA_PIN        GPIO_PIN_8

/* I2C 时序参数（单位: us, 适用于 ~100kHz） */
#define SOFT_I2C_DELAY_HALF     5    /* SCL 半周期            */
#define SOFT_I2C_DELAY_SETUP    2    /* 数据建立时间           */
#define SOFT_I2C_DELAY_HOLD     1    /* 数据保持时间           */

/* GPIO 控制宏（开漏 + 上拉, 写 0 = 拉低,  写 1 = 释放） */
#define SDA_H()   ((SOFT_I2C_SDA_PORT)->BSRR = SOFT_I2C_SDA_PIN)
#define SDA_L()   ((SOFT_I2C_SDA_PORT)->BSRR = (uint32_t)(SOFT_I2C_SDA_PIN) << 16)
#define SCL_H()   ((SOFT_I2C_SCL_PORT)->BSRR = SOFT_I2C_SCL_PIN)
#define SCL_L()   ((SOFT_I2C_SCL_PORT)->BSRR = (uint32_t)(SOFT_I2C_SCL_PIN) << 16)
#define SDA_READ  (((SOFT_I2C_SDA_PORT)->IDR & SOFT_I2C_SDA_PIN) ? 1 : 0)

void soft_i2c_init(void);
void soft_i2c_start(void);
void soft_i2c_stop(void);
uint8_t soft_i2c_write_byte(uint8_t data);
uint8_t soft_i2c_read_byte(uint8_t ack);
void soft_i2c_delay_us(uint32_t us);

#endif
