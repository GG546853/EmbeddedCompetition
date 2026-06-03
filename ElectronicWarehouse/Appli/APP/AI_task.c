#include "app_freertos.h"
#include "AI_task.h"
#include "app_x-cube-ai.h"
#include "imx335.h"
#include "dcmipp.h"
#include "network_f.h"
#include "network_fc.h"
#include "main.h"
#include "rgblcd.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#define DISP_W 800
#define DISP_H 480
#define BOX_THICKNESS 3
#define KP_SIZE 5
#define CMD_BUF_SIZE  64

extern uint8_t g_ltdc_layer2_framebuf[480 * 800 * 3];
extern uint16_t g_ltdc_lcd_framebuf[480 * 800];

static inline void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || x >= DISP_W || y < 0 || y >= DISP_H) return;
    uint32_t off = (y * DISP_W + x) * 3;
    g_ltdc_layer2_framebuf[off + 0] = r;
    g_ltdc_layer2_framebuf[off + 1] = g;
    g_ltdc_layer2_framebuf[off + 2] = b;
}

static void fill_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > DISP_W) w = DISP_W - x;
    if (y + h > DISP_H) h = DISP_H - y;
    if (w <= 0 || h <= 0) return;

    for (int row = 0; row < h; row++) {
        uint32_t off = ((y + row) * DISP_W + x) * 3;
        for (int col = 0; col < w; col++) {
            g_ltdc_layer2_framebuf[off + 0] = r;
            g_ltdc_layer2_framebuf[off + 1] = g;
            g_ltdc_layer2_framebuf[off + 2] = b;
            off += 3;
        }
    }
}

static void draw_box(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b)
{
    fill_rect(x, y, w, BOX_THICKNESS, r, g, b);               /* top */
    fill_rect(x, y + h - BOX_THICKNESS, w, BOX_THICKNESS, r, g, b); /* bottom */
    fill_rect(x, y, BOX_THICKNESS, h, r, g, b);               /* left */
    fill_rect(x + w - BOX_THICKNESS, y, BOX_THICKNESS, h, r, g, b); /* right */
}

static void draw_kp(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    fill_rect(x - KP_SIZE/2, y - KP_SIZE/2, KP_SIZE, KP_SIZE, r, g, b);
}

static void draw_detections_on_display(ai_result_t *result)
{
    memset(g_ltdc_layer2_framebuf, 0, sizeof(g_ltdc_layer2_framebuf));

    for (uint32_t i = 0; i < result->nb_detect; i++) {
        ai_detection_t *d = &result->detections[i];

        int bx = (int)(d->x_center * DISP_W - d->width  * DISP_W * 0.5f);
        int by = (int)(d->y_center * DISP_H - d->height * DISP_H * 0.5f);
        int bw = (int)(d->width  * DISP_W);
        int bh = (int)(d->height * DISP_H);

        draw_box(bx, by, bw, bh, 0, 255, 0);

        for (int k = 0; k < 6; k++) {
            int kx = (int)(d->keypoints[k][0] * DISP_W);
            int ky = (int)(d->keypoints[k][1] * DISP_H);
            draw_kp(kx, ky, 255, 0, 0);
        }
    }

    SCB_CleanInvalidateDCache_by_Addr(
        (uint32_t *)g_ltdc_layer2_framebuf, sizeof(g_ltdc_layer2_framebuf));
}

osThreadId_t AITaskHandle;
const osThreadAttr_t AITask_attributes = {
    .name = "AITask",
    .priority = (osPriority_t) osPriorityNormal + 1,
    .stack_size = 2048 * 4
};

extern DCMIPP_HandleTypeDef hdcmipp;
extern uint8_t *buffer_in;
extern uint8_t *buffer_in_fc;

/* Intermediate buffer for DCMIPP PIPE2 uint8 RGB888 output.
   Must be in .noncacheable section and 32-byte aligned for DMA access. */
uint8_t nn_input_u8[128 * 128 * 3] __attribute__((section(".noncacheable"), aligned(32)));

static const char *kp_names[6] = {
    "LEye", "REye", "Nose", "Mouth", "LEar", "REar"
};
extern osSemaphoreId_t cam_frame_sem;

/* -------------------------------------------------------------------------- */
/*                     Serial Command Parser (uses BSP RX buffer)               */
/* -------------------------------------------------------------------------- */

typedef enum {
    REID_IDLE,
    REID_REGISTER,
    REID_IDENTIFY,
} reid_action_t;

static reid_action_t reid_pending;
static char          reid_name[FACE_NAME_MAX];

/* BSP interrupt-driven RX accumulates into g_uart_rx_buf[].
   g_uart_rx_sta bit15 = line ready (received \r\n). */

static void process_serial_commands(void)
{
    if (!(g_uart_rx_sta & 0x8000)) return;

    uint16_t len = g_uart_rx_sta & 0x3FFF;
    if (len >= CMD_BUF_SIZE) len = CMD_BUF_SIZE - 1;

    char cmd[CMD_BUF_SIZE];
    memcpy(cmd, g_uart_rx_buf, len);
    cmd[len] = '\0';

    g_uart_rx_sta = 0;  /* reset for next line */

    printf("[CMD] %s\r\n", cmd);

    if (strncmp(cmd, "register ", 9) == 0) {
        strncpy(reid_name, cmd + 9, FACE_NAME_MAX - 1);
        reid_name[FACE_NAME_MAX - 1] = '\0';
        reid_pending = REID_REGISTER;
        printf("[REID] Will register face as '%s' on next frame\r\n", reid_name);
    }
    else if (strcmp(cmd, "identify") == 0) {
        reid_pending = REID_IDENTIFY;
        printf("[REID] Will identify face on next frame\r\n");
    }
    else if (strcmp(cmd, "gallery") == 0) {
        ai_face_gallery_print();
    }
    else if (strcmp(cmd, "help") == 0) {
        printf("[HELP] Commands:\r\n");
        printf("  register <name>  -- enroll current face\r\n");
        printf("  identify         -- identify current face\r\n");
        printf("  gallery          -- list registered faces\r\n");
    }
}

/* -------------------------------------------------------------------------- */
/*                     ReID Pipeline (triggered by serial cmd)                 */
/* -------------------------------------------------------------------------- */

/* Reuse nn_input_u8 as face crop buffer (48KB) — the original camera data
   is no longer needed after float32 conversion. */
#define FACE_CROP_BUF  nn_input_u8

static void run_reid_pipeline(ai_result_t *result)
{
    if (reid_pending == REID_IDLE) return;
    if (result->nb_detect == 0) {
        printf("[REID] No face detected, skipping.\r\n");
        reid_pending = REID_IDLE;
        return;
    }

    /* Use the highest-confidence detection */
    ai_detection_t *best = &result->detections[0];

    printf("[REID] DBG1: crop start, box=(%.2f,%.2f,%.2f,%.2f)\r\n",
           best->x_center, best->y_center, best->width, best->height);

    /* Step 1: Crop + resize face from display buffer → 128×128 uint8 */
    ai_crop_resize_face_128(g_ltdc_lcd_framebuf, best, FACE_CROP_BUF);

    printf("[REID] DBG2: crop done, quantize start, fc_in=%p\r\n", buffer_in_fc);

    /* Step 2: uint8 → int8 quantization */
    quantize_u8_to_s8(FACE_CROP_BUF, (int8_t *)buffer_in_fc,
                      LL_ATON_NETWORK_FC_IN_1_SIZE_BYTES);

    printf("[REID] DBG3: quantize done, cache flush\r\n");

    /* Step 3: Cache maintenance for network_fc input */
    SCB_CleanInvalidateDCache_by_Addr(
        (uint32_t *)buffer_in_fc, LL_ATON_NETWORK_FC_IN_1_SIZE_BYTES);

    printf("[REID] DBG4: starting fc inference...\r\n");

    /* Step 4: Run network_fc inference */
    float embedding[FACE_EMBEDDING_DIM];
    ai_face_reid_run(embedding);

    printf("[REID] DBG5: fc inference done\r\n");

    /* DEBUG: print first 8 embedding values */
    printf("[REID-EMB] ");
    for (int i = 0; i < 8; i++) printf("%.4f ", embedding[i]);
    printf("\r\n");

    /* Step 5: Enroll or identify */
    if (reid_pending == REID_REGISTER) {
        ai_face_enroll(embedding, reid_name);
        printf("[REID] Enrolled '%s'\r\n", reid_name);
        /* Draw name on display */
        rgblcd_show_string(10, 10, 200, 16, 16, reid_name, 0x07E0);
    }
    else if (reid_pending == REID_IDENTIFY) {
        char name[FACE_NAME_MAX];
        float dist;
        int idx = ai_face_identify(embedding, name, &dist);
        if (idx >= 0) {
            printf("[REID] Matched: '%s' (dist=%.3f)\r\n", name, dist);
            rgblcd_show_string(10, 30, 200, 16, 16, name, 0x07E0);
        } else {
            printf("[REID] No match (best_dist=%.3f, threshold=%.1f)\r\n",
                   dist, (double)FACE_MATCH_THRESHOLD);
            rgblcd_show_string(10, 30, 200, 16, 16, "Unknown", 0xF800);
        }
    }

    reid_pending = REID_IDLE;
}

/* -------------------------------------------------------------------------- */
/*                               AI Task                                       */
/* -------------------------------------------------------------------------- */

void AI_Task(void *argument)
{
    ai_result_t result;

    /* Wait for camera PIPE1 to be running (Sensor_Task starts it) */
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("\r\n[REID] Face Re-ID ready. Commands: register <name>, identify, gallery, help\r\n");

    for (;;) {
        if(osSemaphoreAcquire(cam_frame_sem, HAL_MAX_DELAY) != osOK)
            while(1);

        /* Cache maintenance: DCMIPP wrote to nn_input_u8 via DMA */
        SCB_CleanInvalidateDCache_by_Addr(
            (uint32_t *)nn_input_u8, sizeof(nn_input_u8));

        /* Software uint8→float32 normalization: nn_input_u8 → buffer_in */
        float *fin = (float *)buffer_in;
        for (uint32_t i = 0; i < 128 * 128 * 3; i++) {
            fin[i] = (float)nn_input_u8[i] / 255.0f;
        }

        /* Cache maintenance: CPU wrote to buffer_in */
        SCB_CleanInvalidateDCache_by_Addr(
            (uint32_t *)buffer_in, LL_ATON_NETWORK_F_IN_1_SIZE_BYTES);

        /* Run NPU inference + post-processing */
        memset(&result, 0, sizeof(result));
        MX_X_CUBE_AI_Process_User(&result);

#if 0
        /* Print results via serial */
        printf("--- Frame ---\r\n");
        printf("Detections: %lu\r\n", result.nb_detect);
        for (uint32_t i = 0; i < result.nb_detect; i++) {
            ai_detection_t *d = &result.detections[i];
            printf("  [%lu] conf=%.2f box=(%.3f,%.3f,%.3f,%.3f)\r\n",
                   i, d->confidence,
                   d->x_center, d->y_center, d->width, d->height);
            printf("      kp: ");
            for (int k = 0; k < 6; k++) {
                printf("%s(%.3f,%.3f)%s",
                       kp_names[k], d->keypoints[k][0], d->keypoints[k][1],
                       (k < 5) ? " " : "\r\n");
            }
        }
#endif

        /* Draw detection boxes + keypoints on LTDC layer 2 overlay */
        draw_detections_on_display(&result);

        /* Check serial commands */
        process_serial_commands();

        /* Run ReID pipeline if triggered */
        run_reid_pipeline(&result);

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
