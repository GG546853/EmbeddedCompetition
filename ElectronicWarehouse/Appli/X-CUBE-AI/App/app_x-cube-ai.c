
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body — BlazeFace Face Detection
  ******************************************************************************
  */
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#include "app_x-cube-ai.h"
#include "main.h"

/* USER CODE BEGIN includes */
#include "core_cm55.h"
#include "network.h"
#include "npu_cache.h"
#include "dma2d.h"
#include "rgblcd.h"
#include "fd_blazeface_anchors_0.h"
#include "fd_blazeface_anchors_1.h"
#include "app_freertos.h"

/* ====================== BlazeFace Post-Processing Constants ====================== */
#define FD_IMG_SIZE              (128)
#define FD_NB_KEYPOINTS          (6)
#define FD_NB_DETECTIONS_0       (512)
#define FD_NB_DETECTIONS_1       (384)
#define FD_TOTAL_BOXES           (FD_NB_DETECTIONS_0 + FD_NB_DETECTIONS_1)
#define FD_MAX_BOXES_LIMIT       (3)
#define FD_CONF_THRESHOLD        (0.8f)
#define FD_IOU_THRESHOLD         (0.5f)
#define FD_BOXE_STRIDE           (FD_NB_KEYPOINTS * 2 + 4)  /* 16 */

/* ====================== Detection Output Structures ====================== */
typedef struct {
    float x, y;
} kpt_t;

typedef struct {
    float x_center, y_center, width, height;
    float conf;
    int   class_index;
    kpt_t keypoints[FD_NB_KEYPOINTS];
    int   keep;
} box_t;

/* ====================== External Declarations ====================== */
extern DCMIPP_HandleTypeDef   hdcmipp;
extern osSemaphoreId_t        cam_frame_sem;
extern DMA2D_HandleTypeDef    hdma2d;
extern uint8_t                g_ltdc_layer2_framebuf[480 * 800 * 3];

/* ====================== Buffers ====================== */
__attribute__((section(".noncacheable"), aligned(32)))
uint8_t  g_ai_cam_buf[FD_IMG_SIZE * FD_IMG_SIZE * 3];

box_t boxes[FD_TOTAL_BOXES];

/* Pointers to NPU-allocated I/O buffers (initialized once after Init_Network) */
static float  *buffer_in_f32;
static float  *buffer_out_f32[4];
static uint32_t buff_in_len;
static uint32_t buff_out_len[4];

/* ====================== Debug flag ====================== */
#define FD_DEBUG_PRINT 1

/* -------------------------------------------------------------------------- */
/*                          Static Helpers                                     */
/* -------------------------------------------------------------------------- */

static inline float sigmoid_f(float x) {
    return 1.0f / (1.0f + expf(-x));
}

static float box_iou(const box_t *a, const box_t *b) {
    float ax1 = a->x_center - a->width  * 0.5f;
    float ay1 = a->y_center - a->height * 0.5f;
    float ax2 = a->x_center + a->width  * 0.5f;
    float ay2 = a->y_center + a->height * 0.5f;

    float bx1 = b->x_center - b->width  * 0.5f;
    float by1 = b->y_center - b->height * 0.5f;
    float bx2 = b->x_center + b->width  * 0.5f;
    float by2 = b->y_center + b->height * 0.5f;

    float ix1 = ax1 > bx1 ? ax1 : bx1;
    float iy1 = ay1 > by1 ? ay1 : by1;
    float ix2 = ax2 < bx2 ? ax2 : bx2;
    float iy2 = ay2 < by2 ? ay2 : by2;

    float iw = ix2 - ix1;
    float ih = iy2 - iy1;
    if (iw <= 0.0f || ih <= 0.0f) return 0.0f;
    float inter = iw * ih;
    float area_a = a->width * a->height;
    float area_b = b->width * b->height;
    float uni = area_a + area_b - inter;
    return (uni > 0.0f) ? (inter / uni) : 0.0f;
}

static int box_conf_cmp(const void *pa, const void *pb) {
    float diff = ((const box_t *)pb)->conf - ((const box_t *)pa)->conf;
    if (diff < 0) return -1;
    if (diff > 0) return  1;
    return 0;
}

/*
 * Decode one detection head (float32 version).
 *  reg_f32  : [nb_det * 16] box regression + keypoints (already float32)
 *  score_f32: [nb_det] raw logit scores (float32, sigmoid NOT yet applied)
 *  anchors  : [nb_det * 2] (x_center, y_center) in [0,1] range
 */
/*
 * Decode one detection head (float32 version).
 *
 * The model's DequantizeLinear uses scale = 1/255 for regression outputs,
 * converting uint8 pixel offsets into [0.0, 1.0] float32.  We multiply by
 * 255.0f to recover the original pixel-space offsets before normalizing by
 * image size.  Classification scores are logits — sigmoid is applied here.
 */
static int decode_head(const float *reg_f32, const float *score_f32,
                       const float *anchors, int nb_det,
                       int start_idx)
{
    float inv_size = 1.0f / (float)FD_IMG_SIZE;
    float th_logit = -logf(1.0f / FD_CONF_THRESHOLD - 1.0f);
    int det_count = start_idx;

    for (int d = 0; d < nb_det; d++) {
        float score = score_f32[d];
        if (score < th_logit) {
            reg_f32  += FD_BOXE_STRIDE;
            continue;
        }

        box_t *b = &boxes[det_count];
        b->conf        = sigmoid_f(score);
        b->class_index = 0;
        b->keep        = 1;

        float ax = anchors[2 * d];
        float ay = anchors[2 * d + 1];

        /* Recover pixel-space offsets (model dequantized with scale=1/255) */
        float dx = reg_f32[0] * 255.0f;
        float dy = reg_f32[1] * 255.0f;
        float dw = reg_f32[2] * 255.0f;
        float dh = reg_f32[3] * 255.0f;

        b->x_center = dx * inv_size + ax;
        b->y_center = dy * inv_size + ay;
        b->width    = dw * inv_size;
        b->height   = dh * inv_size;

        for (int k = 0; k < FD_NB_KEYPOINTS; k++) {
            float kx = reg_f32[4 + 2 * k]     * 255.0f;
            float ky = reg_f32[4 + 2 * k + 1] * 255.0f;
            b->keypoints[k].x = kx * inv_size + ax;
            b->keypoints[k].y = ky * inv_size + ay;
        }

        reg_f32 += FD_BOXE_STRIDE;
        det_count++;
    }
    return det_count;
}

static void cam_rgb888_to_f32(const uint8_t *src, float *dst, int pixel_count) {
    for (int i = 0; i < pixel_count * 3; i++) {
        dst[i] = (float)src[i] * 0.003921569f;
    }
}

/* ====================== Debug helpers ====================== */
static void dump_output_tensor(const char *label, const float *buf, int count) {
    printf("%s [%d]: ", label, count);
    for (int i = 0; i < count && i < 10; i++) {
        printf("%.4f ", (double)buf[i]);
    }
    /* min / max over all values */
    float vmin = buf[0], vmax = buf[0];
    for (int i = 1; i < count; i++) {
        if (buf[i] < vmin) vmin = buf[i];
        if (buf[i] > vmax) vmax = buf[i];
    }
    printf("| min=%.4f max=%.4f\r\n", (double)vmin, (double)vmax);
}


/* Entry points --------------------------------------------------------------*/
LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(Default)

/* USER CODE END includes */


/* -------------------------------------------------------------------------- */
/*                           Low-Power Clock Config                            */
/* -------------------------------------------------------------------------- */
void set_clk_sleep_mode(void)
{
#if defined (CPU_IN_SECURE_STATE)
  __HAL_RCC_DBG_CLK_SLEEP_ENABLE();
#endif
  __HAL_RCC_XSPIPHYCOMP_CLK_SLEEP_ENABLE();

  __HAL_RCC_AXISRAM1_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM2_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM3_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM4_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM5_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM6_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_FLEXRAM_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_CACHEAXIRAM_MEM_CLK_SLEEP_ENABLE();

#if defined (CPU_IN_SECURE_STATE)
  __HAL_RCC_RIFSC_CLK_SLEEP_ENABLE();
  __HAL_RCC_RISAF_CLK_SLEEP_ENABLE();
  __HAL_RCC_IAC_CLK_SLEEP_ENABLE();
#endif

  __HAL_RCC_XSPI1_CLK_SLEEP_ENABLE();
  __HAL_RCC_XSPI2_CLK_SLEEP_ENABLE();
  __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE();
  __HAL_RCC_NPU_CLK_SLEEP_ENABLE();
  __HAL_RCC_USART1_CLK_SLEEP_ENABLE();
}

/* -------------------------------------------------------------------------- */
/*                                 AI Init                                     */
/* -------------------------------------------------------------------------- */
void MX_X_CUBE_AI_Init(void)
{
    set_clk_sleep_mode();
    __HAL_RCC_NPU_CLK_ENABLE();
    __HAL_RCC_NPU_FORCE_RESET();
    __HAL_RCC_NPU_RELEASE_RESET();
    npu_cache_init();

    /* USER CODE BEGIN 5 */
    __HAL_RCC_AXISRAM4_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM5_MEM_CLK_ENABLE();

    RAMCFG_SRAM4_AXI->CR &= ~RAMCFG_CR_SRAMSD;
    RAMCFG_SRAM5_AXI->CR &= ~RAMCFG_CR_SRAMSD;

    __HAL_RCC_RAMCFG_CLK_ENABLE();
    RAMCFG_HandleTypeDef hramcfg = {0};
    hramcfg.Instance =  RAMCFG_SRAM4_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    hramcfg.Instance =  RAMCFG_SRAM5_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);

    __HAL_RCC_XSPI1_CLK_SLEEP_ENABLE();
    __HAL_RCC_XSPI2_CLK_SLEEP_ENABLE();
    __HAL_RCC_NPU_CLK_SLEEP_ENABLE();
    __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE();
    __HAL_RCC_LTDC_CLK_SLEEP_ENABLE();
    __HAL_RCC_DMA2D_CLK_SLEEP_ENABLE();
    __HAL_RCC_DCMIPP_CLK_SLEEP_ENABLE();
    __HAL_RCC_CSI_CLK_SLEEP_ENABLE();

    /* RuntimeInit + Init_Network called ONLY ONCE */
    LL_ATON_RT_RuntimeInit();
    LL_ATON_RT_Init_Network(&NN_Instance_Default);

    /* Cache buffer pointers ONCE after Init_Network */
    const LL_Buffer_InfoTypeDef *ibuf = NN_Interface_Default.input_buffers_info();
    const LL_Buffer_InfoTypeDef *obuf = NN_Interface_Default.output_buffers_info();

    buffer_in_f32 = (float *)LL_Buffer_addr_start(&ibuf[0]);
    buff_in_len   = ibuf[0].offset_end - ibuf[0].offset_start;

    buffer_out_f32[0] = (float *)LL_Buffer_addr_start(&obuf[0]);  /* 512x16 reg_0 */
    buffer_out_f32[1] = (float *)LL_Buffer_addr_start(&obuf[1]);  /* 512x1  cls_0 */
    buffer_out_f32[2] = (float *)LL_Buffer_addr_start(&obuf[2]);  /* 384x1  cls_1 */
    buffer_out_f32[3] = (float *)LL_Buffer_addr_start(&obuf[3]);  /* 384x16 reg_1 */

    for (int o = 0; o < 4; o++) {
        buff_out_len[o] = obuf[o].offset_end - obuf[o].offset_start;
    }

    printf("AI Init OK: in=%p (%luB) out=[%p,%p,%p,%p]\r\n",
           (void *)buffer_in_f32, (unsigned long)buff_in_len,
           (void *)buffer_out_f32[0], (void *)buffer_out_f32[1],
           (void *)buffer_out_f32[2], (void *)buffer_out_f32[3]);
    /* USER CODE END 5 */
}



/* -------------------------------------------------------------------------- */
/*                        AI Inference + Post-Process                          */
/* -------------------------------------------------------------------------- */
void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 6 */
    LL_ATON_RT_RetValues_t ll_aton_rt_ret;

    /* ==================== 1. CAPTURE FRAME ==================== */
    SCB_CleanDCache_by_Addr((uint32_t *)g_ai_cam_buf, sizeof(g_ai_cam_buf));
    SCB_InvalidateDCache_by_Addr((uint32_t *)g_ai_cam_buf, sizeof(g_ai_cam_buf));

    if (HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0,
                                   (uint32_t)g_ai_cam_buf,
                                   DCMIPP_MODE_SNAPSHOT) != HAL_OK) {
        printf("ERROR: DCMIPP PIPE2 Start Failed!\r\n");
        return;
    }

    if (osSemaphoreAcquire(cam_frame_sem, pdMS_TO_TICKS(100)) != osOK) {
        printf("ERROR: Camera frame timeout!\r\n");
        return;
    }

    SCB_InvalidateDCache_by_Addr((uint32_t *)g_ai_cam_buf, sizeof(g_ai_cam_buf));

    /* ==================== 2. PRE-PROCESS: uint8 → float32 ==================== */
    cam_rgb888_to_f32(g_ai_cam_buf, buffer_in_f32, FD_IMG_SIZE * FD_IMG_SIZE);
    SCB_CleanDCache_by_Addr((uint32_t *)buffer_in_f32, buff_in_len);

    /* ==================== 3. NPU INFERENCE ==================== */
    do {
        ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);
        if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
            LL_ATON_OSAL_WFE();
        }
    } while (ll_aton_rt_ret != LL_ATON_RT_DONE);

    /* Invalidate output buffers (NPU wrote to them) */
    for (int o = 0; o < 4; o++) {
        SCB_InvalidateDCache_by_Addr((uint32_t *)buffer_out_f32[o], buff_out_len[o]);
    }

    /* ==================== DEBUG: dump raw output samples ==================== */
#if FD_DEBUG_PRINT
    /* cls_0: 512 scores */
    dump_output_tensor("cls_0", buffer_out_f32[1], 512);
    /* cls_1: 384 scores */
    dump_output_tensor("cls_1", buffer_out_f32[2], 384);
    /* reg_0: first 10 of 512×16 regression values */
    dump_output_tensor("reg_0", buffer_out_f32[0], 512 * 16);
    /* reg_1: first 10 of 384×16 regression values */
    dump_output_tensor("reg_1", buffer_out_f32[3], 384 * 16);

    printf("th_logit=%.4f\r\n", (double)(-logf(1.0f / FD_CONF_THRESHOLD - 1.0f)));
#endif

    /* ==================== 4. BLAZEFACE POST-PROCESSING ==================== */
    int nb_detect = 0;

    nb_detect = decode_head(buffer_out_f32[0], buffer_out_f32[1],
                            g_Anchors_0, FD_NB_DETECTIONS_0, 0);

    nb_detect = decode_head(buffer_out_f32[3], buffer_out_f32[2],
                            g_Anchors_1, FD_NB_DETECTIONS_1, nb_detect);

#if FD_DEBUG_PRINT
    printf("decode_head: %d raw detections (before NMS)\r\n", nb_detect);
    if (nb_detect > 0) {
        printf("  top-3 raw: ");
        for (int i = 0; i < nb_detect && i < 3; i++) {
            printf("[%d] conf=%.4f @ (%.3f,%.3f) %dx%d  ",
                   i, (double)boxes[i].conf,
                   (double)boxes[i].x_center, (double)boxes[i].y_center,
                   (int)(boxes[i].width * 800.0f), (int)(boxes[i].height * 480.0f));
        }
        printf("\r\n");
    }
#endif

    /* qsort: descending by confidence */
    qsort(boxes, nb_detect, sizeof(box_t), box_conf_cmp);

    /* Weighted NMS */
    for (int i = 0; i < nb_detect; i++) {
        if (!boxes[i].keep) continue;
        for (int j = i + 1; j < nb_detect; j++) {
            if (!boxes[j].keep) continue;
            if (box_iou(&boxes[i], &boxes[j]) > FD_IOU_THRESHOLD) {
                boxes[j].keep = 0;
            }
        }
    }

    /* Confidence filtering + limit */
    int final_count = 0;
    for (int i = 0; i < nb_detect; i++) {
        if (boxes[i].keep && boxes[i].conf >= FD_CONF_THRESHOLD) {
            if (final_count < FD_MAX_BOXES_LIMIT) {
                if (final_count != i) {
                    boxes[final_count] = boxes[i];
                }
                final_count++;
            }
        }
    }

#if FD_DEBUG_PRINT
    printf("after NMS+filter: %d final detections\r\n", final_count);
#endif

    /* ==================== 5. DRAW ON LCD LAYER2 ==================== */
    /* Clear Layer2 */
    hdma2d.Init.Mode         = DMA2D_R2M;
    hdma2d.Init.ColorMode    = DMA2D_OUTPUT_RGB888;
    hdma2d.Init.OutputOffset = 0;
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_ConfigLayer(&hdma2d, 1);
    HAL_DMA2D_Start(&hdma2d, 0x00000000,
                    (uint32_t)g_ltdc_layer2_framebuf, 800, 480);
    HAL_DMA2D_PollForTransfer(&hdma2d, 1000);

    for (int i = 0; i < final_count; i++) {
        box_t *b = &boxes[i];

        int bx1 = (int)(b->x_center * 800.0f - b->width  * 800.0f * 0.5f);
        int by1 = (int)(b->y_center * 480.0f - b->height * 480.0f * 0.5f);
        int bx2 = (int)(b->x_center * 800.0f + b->width  * 800.0f * 0.5f);
        int by2 = (int)(b->y_center * 480.0f + b->height * 480.0f * 0.5f);

        if (bx1 < 0) bx1 = 0;  if (bx2 > 800) bx2 = 800;
        if (by1 < 0) by1 = 0;  if (by2 > 480) by2 = 480;
        int bw = bx2 - bx1;  int bh = by2 - by1;
        if (bw <= 0 || bh <= 0) continue;

#if FD_DEBUG_PRINT
        printf("  draw box[%d]: (%d,%d)-(%d,%d) conf=%.4f\r\n",
               i, bx1, by1, bx2, by2, (double)b->conf);
#endif

        /* Red bounding box */
        hdma2d.Init.Mode         = DMA2D_R2M;
        hdma2d.Init.ColorMode    = DMA2D_OUTPUT_RGB888;
        hdma2d.Init.RedBlueSwap  = DMA2D_RB_REGULAR;

        /* Top */
        hdma2d.Init.OutputOffset = 800 - bw;
        HAL_DMA2D_Init(&hdma2d);
        HAL_DMA2D_Start(&hdma2d, 0x00FF0000,
                        (uint32_t)&g_ltdc_layer2_framebuf[(by1 * 800 + bx1) * 3],
                        bw, 1);
        HAL_DMA2D_PollForTransfer(&hdma2d, 100);

        /* Bottom */
        if (by2 - 1 > by1) {
            hdma2d.Init.OutputOffset = 800 - bw;
            HAL_DMA2D_Init(&hdma2d);
            HAL_DMA2D_Start(&hdma2d, 0x00FF0000,
                            (uint32_t)&g_ltdc_layer2_framebuf[((by2 - 1) * 800 + bx1) * 3],
                            bw, 1);
            HAL_DMA2D_PollForTransfer(&hdma2d, 100);
        }

        /* Left */
        hdma2d.Init.OutputOffset = 800 - 1;
        HAL_DMA2D_Init(&hdma2d);
        HAL_DMA2D_Start(&hdma2d, 0x00FF0000,
                        (uint32_t)&g_ltdc_layer2_framebuf[(by1 * 800 + bx1) * 3],
                        1, bh);
        HAL_DMA2D_PollForTransfer(&hdma2d, 100);

        /* Right */
        if (bx2 - 1 > bx1) {
            hdma2d.Init.OutputOffset = 800 - 1;
            HAL_DMA2D_Init(&hdma2d);
            HAL_DMA2D_Start(&hdma2d, 0x00FF0000,
                            (uint32_t)&g_ltdc_layer2_framebuf[(by1 * 800 + (bx2 - 1)) * 3],
                            1, bh);
            HAL_DMA2D_PollForTransfer(&hdma2d, 100);
        }

        /* Green keypoints */
        for (int k = 0; k < FD_NB_KEYPOINTS; k++) {
            int kx = (int)(b->keypoints[k].x * 800.0f);
            int ky = (int)(b->keypoints[k].y * 480.0f);
            if (kx < 1 || kx >= 799 || ky < 1 || ky >= 479) continue;

            for (int dy = -1; dy <= 1; dy++) {
                int py = ky + dy;
                if (py < 0 || py >= 480) continue;
                hdma2d.Init.OutputOffset = 800 - 3;
                HAL_DMA2D_Init(&hdma2d);
                HAL_DMA2D_Start(&hdma2d, 0x0000FF00,
                                (uint32_t)&g_ltdc_layer2_framebuf[(py * 800 + (kx - 1)) * 3],
                                3, 1);
                HAL_DMA2D_PollForTransfer(&hdma2d, 100);
            }
        }
    }

    printf("FD: %d face(s)\r\n", final_count);

    /* Reset network for next frame (NOT RuntimeInit!) */
    LL_ATON_RT_Reset_Network(&NN_Instance_Default);

    vTaskDelay(pdMS_TO_TICKS(10));
    /* USER CODE END 6 */
}

#ifdef __cplusplus
}
#endif
