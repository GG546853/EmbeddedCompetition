#include "UART_protocol.h"

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
