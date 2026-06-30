#ifndef __UART_PROTOCOL_H
#define __UART_PROTOCOL_H

#include <stdint.h>
#include "Outbound_task.h"

#define FRAME_SYNC           0xA5

/* STM32 -> ESP32 */
#define FRAME_TYPE_BARCODE   0x18

/* ESP32 -> STM32 */
#define FRAME_TYPE_INVENTORY 0x07



uint8_t protocol_build_frame(uint8_t type, const uint8_t *payload, uint8_t len, uint8_t *out);

typedef struct {
    uint8_t state;
    uint8_t type;
    uint8_t len;
    uint8_t idx;
    uint8_t payload[256];
    uint8_t chk;
} FrameParser;

void frame_parser_init(FrameParser *fp);
int  frame_parser_feed(FrameParser *fp, uint8_t byte);

#endif
