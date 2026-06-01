#include "app_freertos.h"
#include "AI_task.h"
#include "app_x-cube-ai.h"
#include "imx335.h"
#include "dcmipp.h"
#include "network_f.h"
#include "main.h"
#include "rgblcd.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DISP_W 800
#define DISP_H 480
#define BOX_THICKNESS 3
#define KP_SIZE 5

extern uint8_t g_ltdc_layer2_framebuf[480 * 800 * 3];

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

/* Intermediate buffer for DCMIPP PIPE2 uint8 RGB888 output.
   Must be in .noncacheable section and 32-byte aligned for DMA access. */
uint8_t nn_input_u8[128 * 128 * 3] __attribute__((section(".noncacheable"), aligned(32)));

static const char *kp_names[6] = {
    "LEye", "REye", "Nose", "Mouth", "LEar", "REar"
};
extern osSemaphoreId_t cam_frame_sem;
void AI_Task(void *argument)
{
    ai_result_t result;

    /* Wait for camera PIPE1 to be running (Sensor_Task starts it) */
    vTaskDelay(pdMS_TO_TICKS(1000));

    for (;;) {
        /* 1. Start PIPE2 snapshot capture 128x128 → intermediate uint8 buffer */
//        HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0,
//                                   (uint32_t)nn_input_u8, DCMIPP_MODE_SNAPSHOT);

        /* 2. Wait for frame completion.
             PIPE1 runs at ~30fps continuous, so the frame counter ticks
             every ~33ms. PIPE2 snapshot completes in one frame cycle. */
//        uint32_t last_count = imx335_get_capture_frame_count();
//        uint32_t timeout = 100; /* 100ms max wait */
//        while (imx335_get_capture_frame_count() == last_count && timeout > 0) {
//            vTaskDelay(pdMS_TO_TICKS(1));
//            timeout--;
//        }
        if(osSemaphoreAcquire(cam_frame_sem, HAL_MAX_DELAY) != osOK)
        	while(1);
        /* 3. Stop PIPE2 (safe even if already auto-stopped by snapshot mode) */
//        HAL_DCMIPP_CSI_PIPE_Stop(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0);

        /* 4. Cache maintenance: DCMIPP wrote to nn_input_u8 via DMA */
        SCB_CleanInvalidateDCache_by_Addr(
            (uint32_t *)nn_input_u8, sizeof(nn_input_u8));

        /* 5. Software uint8→float32 normalization: nn_input_u8 → buffer_in */
        float *fin = (float *)buffer_in;
        for (uint32_t i = 0; i < 128 * 128 * 3; i++) {
            fin[i] = (float)nn_input_u8[i] / 255.0f;
        }

        /* DEBUG: print first 20 raw pixels and normalized floats */
        printf("[DBG-CAM] u8: ");
        for (int i = 0; i < 20; i++) printf("%u ", nn_input_u8[i]);
        printf("\r\n");
        printf("[DBG-NRM] f32: ");
        for (int i = 0; i < 10; i++) printf("%.4f ", fin[i]);
        printf("\r\n");

        /* 6. Cache maintenance: CPU wrote to buffer_in */
        SCB_CleanInvalidateDCache_by_Addr(
            (uint32_t *)buffer_in, LL_ATON_NETWORK_F_IN_1_SIZE_BYTES);

        /* 7. Run NPU inference + post-processing */
        memset(&result, 0, sizeof(result));
        MX_X_CUBE_AI_Process_User(&result);

        /* 8. Print results via serial */
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

        /* 9. Draw detection boxes + keypoints on LTDC layer 2 overlay */
        draw_detections_on_display(&result);
        vTaskDelay(pdMS_TO_TICKS(1));
       // vTaskDelay(pdMS_TO_TICKS(100));
    }
}
