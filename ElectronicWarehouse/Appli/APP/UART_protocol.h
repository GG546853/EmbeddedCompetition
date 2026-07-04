#ifndef __UART_PROTOCOL_H
#define __UART_PROTOCOL_H

#include <stdint.h>


#define FRAME_SYNC           0xA5

/* STM32 -> ESP32 */
#define FRAME_TYPE_TEMP      0x11
#define FRAME_TYPE_HUMID     0x12
#define FRAME_TYPE_VOLT      0x13
#define FRAME_TYPE_FACE_ENR  0x14
#define FRAME_TYPE_FACE_EVT  0x15
#define FRAME_TYPE_OUTBOUND  0x16
#define FRAME_TYPE_INBOUND   0x17
#define FRAME_TYPE_BARCODE   0x18
#define FRAME_TYPE_RPT_STORE  0x19

/* ESP32 -> STM32 */
#define FRAME_TYPE_LED        0x01
#define FRAME_TYPE_DRAWER     0x02
#define FRAME_TYPE_FACE_CLEAR 0x03
#define FRAME_TYPE_ALERT      0x04
#define FRAME_TYPE_HEARTBEAT  0x05
#define FRAME_TYPE_INVENTORY  0x06
#define FRAME_TYPE_STORE      0x07
#define FRAME_TYPE_TIME       0x08

uint8_t protocol_build_frame(uint8_t type, const uint8_t *payload, uint8_t len, uint8_t *out);

typedef void (*FrameHandler)(const uint8_t *payload, uint8_t len);

typedef struct {
    uint8_t       type;
    FrameHandler  handler;
} FrameDispatchEntry;

void uart4_dispatch_frame(uint8_t type, const uint8_t *payload, uint8_t len);

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

/* Send helpers: payload → frame → UART4 */
void uart4_send_temperature(float val);
void uart4_send_humidity(float val);
void uart4_send_voltage(float val);
void uart4_send_face_enroll(const char *id);
void uart4_send_face_event(const char *event);
void uart4_send_outbound(const char *info);
void uart4_send_inbound(const char *info);
void uart4_send_barcode(const uint8_t *data, uint8_t len);
void uart4_send_store(const char *location);

#endif
