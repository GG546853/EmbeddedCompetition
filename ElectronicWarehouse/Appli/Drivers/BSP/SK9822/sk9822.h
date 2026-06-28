#ifndef __SK9822_H
#define __SK9822_H

#include "main.h"
#include "spi.h"

#define SK9822_NUM_LEDS        112
#define SK9822_NUM_GROUPS      28
#define SK9822_LEDS_PER_GROUP  4

#define SK9822_BRIGHTNESS_MAX  31

void SK9822_Init(void);
void SK9822_SetLED(uint16_t led_idx, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);
void SK9822_SetGroup(uint8_t group, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);
void SK9822_SetAll(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);
void SK9822_Clear(void);
void SK9822_Transmit(void);

#endif
