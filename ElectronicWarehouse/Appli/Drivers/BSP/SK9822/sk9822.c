#include "sk9822.h"
#include <string.h>

/* End frame = max(4, ceil(N/16)) bytes of 0xFF. For 112 LEDs: max(4,7) = 7 */
#define END_FRAME_SIZE  (((SK9822_NUM_LEDS + 15) / 16) < 4 ? 4 : ((SK9822_NUM_LEDS + 15) / 16))
#define START_FRAME_SIZE 4

#define BUF_SIZE  (START_FRAME_SIZE + SK9822_NUM_LEDS * 4 + END_FRAME_SIZE)

static uint8_t tx_buf[BUF_SIZE];

void SK9822_Init(void)
{
    memset(tx_buf, 0, BUF_SIZE);

    /* Start frame: 32 zero bits */
    for (int i = 0; i < START_FRAME_SIZE; i++) {
        tx_buf[i] = 0x00;
    }

    /* End frame: ceil(N/16) bytes of 0xFF, min 4 */
    for (int i = 0; i < END_FRAME_SIZE; i++) {
        tx_buf[BUF_SIZE - END_FRAME_SIZE + i] = 0xFF;
    }

    /* All LEDs off initially */
    SK9822_Clear();
    SK9822_Transmit();
}

void SK9822_SetLED(uint16_t led_idx, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
    if (led_idx >= SK9822_NUM_LEDS) return;

    uint16_t idx = START_FRAME_SIZE + led_idx * 4;
    tx_buf[idx]     = 0xE0 | (brightness & 0x1F);
    tx_buf[idx + 1] = b;
    tx_buf[idx + 2] = g;
    tx_buf[idx + 3] = r;
}

void SK9822_SetGroup(uint8_t group, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
    if (group >= SK9822_NUM_GROUPS) return;

    uint16_t start_led = group * SK9822_LEDS_PER_GROUP;
    for (uint8_t i = 0; i < SK9822_LEDS_PER_GROUP; i++) {
        SK9822_SetLED(start_led + i, r, g, b, brightness);
    }
}

void SK9822_SetAll(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
    for (uint16_t i = 0; i < SK9822_NUM_LEDS; i++) {
        SK9822_SetLED(i, r, g, b, brightness);
    }
}

void SK9822_Clear(void)
{
    for (uint16_t i = 0; i < SK9822_NUM_LEDS; i++) {
        uint16_t idx = START_FRAME_SIZE + i * 4;
        tx_buf[idx]     = 0xE0;
        tx_buf[idx + 1] = 0;
        tx_buf[idx + 2] = 0;
        tx_buf[idx + 3] = 0;
    }
}

void SK9822_Transmit(void)
{
    HAL_SPI_Transmit(&hspi5, tx_buf, BUF_SIZE, HAL_MAX_DELAY);
}
