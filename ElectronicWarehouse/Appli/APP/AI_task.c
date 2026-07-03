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
#include "dma2d.h"
#define DISP_W 800
#define DISP_H 480
#define BOX_THICKNESS 3
#define KP_SIZE 5
#define CMD_BUF_SIZE  64

extern uint8_t g_ltdc_framebuf[480 * 800 * 3];

#define RGB888(r, g, b)  (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

volatile int g_trigger_identify = 0;


static void fill_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > DISP_W) w = DISP_W - x;
    if (y + h > DISP_H) h = DISP_H - y;
    if (w <= 0 || h <= 0) return;

    extern osMutexId_t dma2d_mutex;
    if (dma2d_mutex != NULL) {
        osMutexAcquire(dma2d_mutex, osWaitForever);
    }
    hdma2d.Init.Mode = DMA2D_R2M;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
    hdma2d.Init.OutputOffset = DISP_W - w;
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_Start(&hdma2d, RGB888(r, g, b),
                    (uint32_t)&g_ltdc_framebuf[(y * DISP_W + x) * 3], w, h);
    HAL_DMA2D_PollForTransfer(&hdma2d, 50);
    if (dma2d_mutex != NULL) {
        osMutexRelease(dma2d_mutex);
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

/* DCMIPP 每帧覆盖整个缓冲区，旧检测框自然被擦除 */
static void draw_one_detection(ai_detection_t *d, uint8_t r, uint8_t g, uint8_t b)
{
    int bx = (int)(d->x_center * DISP_W - d->width  * DISP_W * 0.5f);
    int by = (int)(d->y_center * DISP_H - d->height * DISP_H * 0.5f);
    int bw = (int)(d->width  * DISP_W);
    int bh = (int)(d->height * DISP_H);

    draw_box(bx, by, bw, bh, r, g, b);

//    for (int k = 0; k < 6; k++) {
//        int kx = (int)(d->keypoints[k][0] * DISP_W);
//        int ky = (int)(d->keypoints[k][1] * DISP_H);
//        draw_kp(kx, ky, r, g, b);
//    }
}

static void draw_detections_on_display(ai_result_t *result)
{
	if (result->nb_detect == 0) return;
    /* 暂停 PIPE1 连续写入，防止 DCMIPP 新帧覆盖正在绘制的框 */
    HAL_DCMIPP_CSI_PIPE_Stop(&hdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0);
    for (uint32_t i = 0; i < result->nb_detect; i++) {
        draw_one_detection(&result->detections[i], 0, 255, 0);
    }
    HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0,
    (uint32_t)g_ltdc_framebuf, DCMIPP_MODE_CONTINUOUS);
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

reid_action_t reid_pending;
static char          reid_name[FACE_NAME_MAX];
int           g_capture_pending = 0; /* set by 'register' or 's' cmd, consumed by pipeline */

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
        g_capture_pending = 1;  /* first capture at current angle */
        printf("[REID] Enrolling '%s' — send 's' for next sample\r\n", reid_name);
    }
    else if (strcmp(cmd, "s") == 0) {
        if (reid_pending == REID_REGISTER) {
            g_capture_pending = 1;
        } else {
            printf("[REID] Not enrolling — use 'register <name>' first\r\n");
        }
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
        printf("  register <name>  -- start enrolling current face\r\n");
        printf("  s                -- capture one sample (during enrollment)\r\n");
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
        /* In multi-enroll mode keep waiting for a face; otherwise skip. */
        if (reid_pending != REID_REGISTER)
            reid_pending = REID_IDLE;
        return;
    }
    /* Waiting for 's' trigger — don't run inference until then */
    if (reid_pending == REID_REGISTER && !g_capture_pending)
        return;

    /* Use the highest-confidence detection */
    ai_detection_t *best = &result->detections[0];

    printf("[REID] DBG1: crop start, box=(%.2f,%.2f,%.2f,%.2f)\r\n",
           best->x_center, best->y_center, best->width, best->height);

    /* Step 1: Crop + resize face from display buffer → 128×128 uint8 */
    ai_crop_resize_face_112(g_ltdc_framebuf, best, FACE_CROP_BUF);

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
        if (g_capture_pending) {
            g_capture_pending = 0;
            int remaining = ai_face_enroll_multi(embedding, reid_name);
            if (remaining == 0) {
                reid_pending = REID_IDLE;
                printf("[REID] Enrolled '%s'\r\n", reid_name);
            }
            /* else: keep REID_REGISTER; wait for next 's' */
        }
    }
    else if (reid_pending == REID_IDENTIFY) {
        char name[FACE_NAME_MAX];
        float dist;
        int idx = ai_face_identify(embedding, name, &dist);
        if (idx >= 0) {
            printf("[REID] Matched: '%s' (dist=%.3f)\r\n", name, dist);
        } else {
            printf("[REID] No match (dist=%.3f)\r\n", dist);
        }
    }

    if (reid_pending == REID_IDENTIFY)
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

        if(g_trigger_identify == 1)
        {
        	g_trigger_identify = 0;
        	reid_pending = REID_IDENTIFY;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
