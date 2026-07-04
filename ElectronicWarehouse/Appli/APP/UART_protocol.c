#include "UART_protocol.h"
#include "usart.h"
#include <string.h>
uint8_t protocol_build_frame(uint8_t type, const uint8_t *payload, uint8_t len, uint8_t *out)
{
    out[0] = FRAME_SYNC;
    out[1] = type;
    out[2] = len;

    uint8_t chk = type ^ len;
    for (uint8_t i = 0; i < len; i++) {
        out[3 + i] = payload[i];
        chk ^= payload[i];
    }
    out[3 + len] = chk;

    return 4 + len;
}

void frame_parser_init(FrameParser *fp)
{
    fp->state = 0;
    fp->type  = 0;
    fp->len   = 0;
    fp->idx   = 0;
    fp->chk   = 0;
}

int frame_parser_feed(FrameParser *fp, uint8_t byte)
{
    switch (fp->state) {
    case 0: /* WAIT_SYNC */
        if (byte == FRAME_SYNC) {
            fp->state = 1;
        }
        break;
    case 1: /* WAIT_TYPE */
        fp->type = byte;
        fp->chk  = byte;
        fp->state = 2;
        break;
    case 2: /* WAIT_LEN */
        fp->len  = byte;
        fp->chk ^= byte;
        fp->idx  = 0;
        fp->state = (byte == 0) ? 4 : 3;
        break;
    case 3: /* WAIT_PAYLOAD */
        fp->payload[fp->idx] = byte;
        fp->chk ^= byte;
        fp->idx++;
        if (fp->idx >= fp->len) {
            fp->state = 4;
        }
        break;
    case 4: /* WAIT_CHK */
        fp->state = 0;
        if (byte == fp->chk) {
            return 1;
        }
        return -1;
    }
    return 0;
}

/* ---- Dispatch table ---- */
extern void handle_led(const uint8_t *payload, uint8_t len);
extern void handle_drawer(const uint8_t *payload, uint8_t len);
extern void handle_face_clear(const uint8_t *payload, uint8_t len);
extern void handle_alert(const uint8_t *payload, uint8_t len);
extern void handle_heartbeat(const uint8_t *payload, uint8_t len);
extern void handle_inventory(const uint8_t *payload, uint8_t len);
extern void handle_time(const uint8_t *payload, uint8_t len);
extern void handle_store(const uint8_t *payload, uint8_t len);

static const FrameDispatchEntry dispatch_table[] = {
    { FRAME_TYPE_LED,        handle_led },
    { FRAME_TYPE_DRAWER,     handle_drawer },
    { FRAME_TYPE_FACE_CLEAR, handle_face_clear },
    { FRAME_TYPE_ALERT,      handle_alert },
    { FRAME_TYPE_HEARTBEAT,  handle_heartbeat },
    { FRAME_TYPE_INVENTORY,  handle_inventory },
    { FRAME_TYPE_STORE,      handle_store },
    { FRAME_TYPE_TIME,       handle_time },
};

void uart4_dispatch_frame(uint8_t type, const uint8_t *payload, uint8_t len)
{
    uint8_t n = sizeof(dispatch_table) / sizeof(dispatch_table[0]);
    for (uint8_t i = 0; i < n; i++) {
        if (dispatch_table[i].type == type) {
            dispatch_table[i].handler(payload, len);
            return;
        }
    }
}

/* ---- UART4 send helpers ---- */

static void uart4_send_frame(uint8_t type, const uint8_t *payload, uint8_t len)
{
    uint8_t frame[259];
    uint8_t frame_len = protocol_build_frame(type, payload, len, frame);
    HAL_UART_Transmit(&huart4, frame, frame_len, 100);
}

void uart4_send_temperature(float val)
{
    uint16_t raw = (uint16_t)(val * 10.0f + 0.5f);
    uint8_t  p[] = { raw >> 8, raw & 0xFF };
    uart4_send_frame(FRAME_TYPE_TEMP, p, 2);
}

void uart4_send_humidity(float val)
{
    uint16_t raw = (uint16_t)(val * 10.0f + 0.5f);
    uint8_t  p[] = { raw >> 8, raw & 0xFF };
    uart4_send_frame(FRAME_TYPE_HUMID, p, 2);
}

void uart4_send_voltage(float val)
{
    uint16_t raw = (uint16_t)(val * 10.0f + 0.5f);
    uint8_t  p[] = { raw >> 8, raw & 0xFF };
    uart4_send_frame(FRAME_TYPE_VOLT, p, 2);
}

void uart4_send_face_enroll(const char *id)
{
    uint8_t len = (uint8_t)strlen(id);
    uart4_send_frame(FRAME_TYPE_FACE_ENR, (const uint8_t *)id, len);
}

void uart4_send_face_event(const char *event)
{
    uint8_t len = (uint8_t)strlen(event);
    uart4_send_frame(FRAME_TYPE_FACE_EVT, (const uint8_t *)event, len);
}

void uart4_send_outbound(const char *info)
{
    uint8_t len = (uint8_t)strlen(info);
    uart4_send_frame(FRAME_TYPE_OUTBOUND, (const uint8_t *)info, len);
}

void uart4_send_inbound(const char *info)
{
    uint8_t len = (uint8_t)strlen(info);
    uart4_send_frame(FRAME_TYPE_INBOUND, (const uint8_t *)info, len);
}

void uart4_send_barcode(const uint8_t *data, uint8_t len)
{
    uart4_send_frame(FRAME_TYPE_BARCODE, data, len);
}

void uart4_send_store(const char *location)
{
    uint8_t len = (uint8_t)strlen(location);
    uart4_send_frame(FRAME_TYPE_RPT_STORE, (const uint8_t *)location, len);
}
