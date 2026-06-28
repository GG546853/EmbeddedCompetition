#ifndef __AHT10_H
#define __AHT10_H

#include "main.h"

/* AHT10 7 位 I2C 地址 */
#define AHT10_ADDR          0x38

uint8_t aht10_init(void);
uint8_t aht10_read(float *humidity, float *temperature);

#endif
