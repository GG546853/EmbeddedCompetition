
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

 /*
  * Description
  *   v1.0 - Minimum template to show how to use the Neural ART Embedded Client API
  *          Re-target of the printf function is out-of-scope.
  *
  *   For more information, see the embeded documentation:
  *
  *       [1] %X_CUBE_AI_DIR%/Documentation/index.html
  *
  *   X_CUBE_AI_DIR indicates the location where the X-CUBE-AI pack is installed
  *   typical : C:\Users\[user_name]\STM32Cube\Repository\STMicroelectronics\X-CUBE-AI\7.1.0
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "main.h"

/* USER CODE BEGIN includes */
#include "core_cm55.h"
#include "Sensor_task.h"
#include "RGBLED_task.h"
#include "rgblcd.h"
#include "network.h"
#include "npu_cache.h"
#include "dma2d.h"
#include <math.h>
#include "fd_blazeface_anchors_0.h"
#include "fd_blazeface_anchors_1.h"

#define AI_BLAZEFACE_IMG_SIZE       128
#define AI_BLAZEFACE_NB_KEYPOINTS   6
#define AI_BLAZEFACE_OUT_0_NB_BOXES 512
#define AI_BLAZEFACE_OUT_1_NB_BOXES 384
#define AI_BLAZEFACE_TOTAL_BOXES    (AI_BLAZEFACE_OUT_0_NB_BOXES + AI_BLAZEFACE_OUT_1_NB_BOXES)
#define AI_BLAZEFACE_CONF_THRESHOLD 0.5f
#define AI_BLAZEFACE_IOU_THRESHOLD  0.5f
#define AI_BLAZEFACE_MAX_BOXES      5

typedef struct {
    float x_center;
    float y_center;
    float width;
    float height;
    float conf;
} BlazeFaceBox;

extern uint8_t g_ltdc_layer2_framebuf[480 * 800 * 3];
extern DCMIPP_HandleTypeDef hdcmipp;
extern osSemaphoreId_t cam_frame_sem;
extern DMA2D_HandleTypeDef hdma2d;

/* DCMIPP capture buffer: uint8 RGB888, 128x128x3 */
__attribute__((section(".camera_buf")))
__attribute__((aligned(32)))
uint8_t g_ai_cam_buf[AI_BLAZEFACE_IMG_SIZE * AI_BLAZEFACE_IMG_SIZE * 3];

__attribute__((section(".camera_buf")))
__attribute__((aligned(32)))
BlazeFaceBox boxes[AI_BLAZEFACE_TOTAL_BOXES];

/* Helper: sort 4 output indices by ascending buffer size */
static void sort_output_indices(size_t *indices, const LL_Buffer_InfoTypeDef *obuffers, int n)
{
    for (int i = 0; i < n; i++) indices[i] = i;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            uint32_t s_j  = obuffers[indices[j]].offset_end - obuffers[indices[j]].offset_start;
            uint32_t s_j1 = obuffers[indices[j+1]].offset_end - obuffers[indices[j+1]].offset_start;
            if (s_j > s_j1) {
                size_t tmp = indices[j];
                indices[j] = indices[j+1];
                indices[j+1] = tmp;
            }
        }
    }
}

/* Decode one stride: threshold logits, then sigmoid + decode with anchors */
static int blazeface_decode_stride(BlazeFaceBox *out, const float *raw_boxes,
                                    const float *scores, const float *anchors,
                                    int n_anchors, float conf_thr)
{
    float inv_size = 1.0f / (float)AI_BLAZEFACE_IMG_SIZE;
    int box_stride = AI_BLAZEFACE_NB_KEYPOINTS * 2 + 4; // x,y,w,h + 6*(kx,ky) = 16
    int count = 0;

    /* Pre-compute logit threshold: sigmoid(logit_thr) = conf_thr */
    float logit_thr = -logf(1.0f / conf_thr - 1.0f);

    for (int i = 0; i < n_anchors; i++) {
        if (scores[i] > logit_thr) {
            float prob = 1.0f / (1.0f + expf(-scores[i]));
            out[count].x_center = raw_boxes[i * box_stride + 0] * inv_size + anchors[i * 2];
            out[count].y_center = raw_boxes[i * box_stride + 1] * inv_size + anchors[i * 2 + 1];
            out[count].width    = raw_boxes[i * box_stride + 2] * inv_size;
            out[count].height   = raw_boxes[i * box_stride + 3] * inv_size;
            out[count].conf     = prob;
            count++;
        }
    }
    return count;
}

/* IoU between two center-format boxes */
static float blazeface_iou(BlazeFaceBox *a, BlazeFaceBox *b)
{
    float ax1 = a->x_center - a->width * 0.5f;
    float ay1 = a->y_center - a->height * 0.5f;
    float ax2 = a->x_center + a->width * 0.5f;
    float ay2 = a->y_center + a->height * 0.5f;
    float bx1 = b->x_center - b->width * 0.5f;
    float by1 = b->y_center - b->height * 0.5f;
    float bx2 = b->x_center + b->width * 0.5f;
    float by2 = b->y_center + b->height * 0.5f;

    float iw = ax2 < bx2 ? ax2 : bx2;
    float iw2 = ax1 > bx1 ? ax1 : bx1;
    iw -= iw2;
    if (iw <= 0.0f) return 0.0f;

    float ih = ay2 < by2 ? ay2 : by2;
    float ih2 = ay1 > by1 ? ay1 : by1;
    ih -= ih2;
    if (ih <= 0.0f) return 0.0f;

    float inter = iw * ih;
    float area_a = a->width * a->height;
    float area_b = b->width * b->height;
    return inter / (area_a + area_b - inter);
}

/* NMS: sort by confidence descending, suppress overlapping boxes */
static int blazeface_nms(BlazeFaceBox *boxes, int count, float iou_thr, int max_boxes)
{
    /* Bubble sort by conf descending */
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (boxes[j].conf < boxes[j+1].conf) {
                BlazeFaceBox tmp = boxes[j];
                boxes[j] = boxes[j+1];
                boxes[j+1] = tmp;
            }
        }
    }

    int kept[AI_BLAZEFACE_MAX_BOXES];
    int n_kept = 0;

    for (int i = 0; i < count && n_kept < max_boxes; i++) {
        int keep = 1;
        for (int j = 0; j < n_kept; j++) {
            if (blazeface_iou(&boxes[i], &boxes[kept[j]]) > iou_thr) {
                keep = 0;
                break;
            }
        }
        if (keep) {
            kept[n_kept++] = i;
        }
    }

    /* Compact kept boxes to front */
    for (int i = 0; i < n_kept; i++) {
        boxes[i] = boxes[kept[i]];
    }
    return n_kept;
}

/* Draw a 1-pixel border rectangle (4 lines) */
static void dma2d_draw_box_border(int x1, int y1, int x2, int y2, uint32_t color)
{
    int w = x2 - x1;
    int h = y2 - y1;
    if (w <= 0 || h <= 0) return;

    /* top */
    hdma2d.Init.Mode          = DMA2D_R2M;
    hdma2d.Init.ColorMode     = DMA2D_OUTPUT_RGB888;
    hdma2d.Init.OutputOffset  = 800 - w;
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_Start(&hdma2d, color,
                    (uint32_t)(g_ltdc_layer2_framebuf + (y1 * 800 + x1) * 3), w, 1);
    while (HAL_DMA2D_PollForTransfer(&hdma2d, 100) != HAL_OK) {}

    /* bottom */
    hdma2d.Init.OutputOffset  = 800 - w;
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_Start(&hdma2d, color,
                    (uint32_t)(g_ltdc_layer2_framebuf + (y2 * 800 + x1) * 3), w, 1);
    while (HAL_DMA2D_PollForTransfer(&hdma2d, 100) != HAL_OK) {}

    /* left */
    hdma2d.Init.OutputOffset  = 800 - 1;
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_Start(&hdma2d, color,
                    (uint32_t)(g_ltdc_layer2_framebuf + (y1 * 800 + x1) * 3), 1, h);
    while (HAL_DMA2D_PollForTransfer(&hdma2d, 100) != HAL_OK) {}

    /* right */
    HAL_DMA2D_Start(&hdma2d, color,
                    (uint32_t)(g_ltdc_layer2_framebuf + (y1 * 800 + x2) * 3), 1, h);
    while (HAL_DMA2D_PollForTransfer(&hdma2d, 100) != HAL_OK) {}
}

/* USER CODE END includes */

/* Entry points --------------------------------------------------------------*/

LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(Default)
uint8_t *buffer_in;
uint8_t *buffer_out;

void set_clk_sleep_mode(void)
{
  /* Leave clocks enabled in Low Power modes */
  // Low-power clock enable misc
#if defined (CPU_IN_SECURE_STATE)
  __HAL_RCC_DBG_CLK_SLEEP_ENABLE();
#endif
  __HAL_RCC_XSPIPHYCOMP_CLK_SLEEP_ENABLE();

  // Low-power clock enable for memories
  __HAL_RCC_AXISRAM1_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM2_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM3_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM4_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM5_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM6_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_FLEXRAM_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_CACHEAXIRAM_MEM_CLK_SLEEP_ENABLE();
  // LP clock AHB1: None
  // LP clock AHB2: None
  // LP clock AHB3
#if defined (CPU_IN_SECURE_STATE)
  __HAL_RCC_RIFSC_CLK_SLEEP_ENABLE();
  __HAL_RCC_RISAF_CLK_SLEEP_ENABLE();
  __HAL_RCC_IAC_CLK_SLEEP_ENABLE();
#endif
  // LP clock AHB4: None
  // LP clocks AHB5
  __HAL_RCC_XSPI1_CLK_SLEEP_ENABLE();
  __HAL_RCC_XSPI2_CLK_SLEEP_ENABLE();
  __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE();
  __HAL_RCC_NPU_CLK_SLEEP_ENABLE();
  // LP clocks APB1: None
  // LP clocks APB2
  __HAL_RCC_USART1_CLK_SLEEP_ENABLE();
  // LP clocks APB4: None
  // LP clocks APB5: None
}

void MX_X_CUBE_AI_Init(void)
{
    __HAL_RCC_AXISRAM2_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM3_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM4_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM5_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM6_MEM_CLK_ENABLE();
    RAMCFG_SRAM2_AXI->CR &= ~RAMCFG_CR_SRAMSD;
    RAMCFG_SRAM3_AXI->CR &= ~RAMCFG_CR_SRAMSD;
    RAMCFG_SRAM4_AXI->CR &= ~RAMCFG_CR_SRAMSD;
    RAMCFG_SRAM5_AXI->CR &= ~RAMCFG_CR_SRAMSD;
    RAMCFG_SRAM6_AXI->CR &= ~RAMCFG_CR_SRAMSD;
    set_clk_sleep_mode();
    __HAL_RCC_NPU_CLK_ENABLE();
    __HAL_RCC_NPU_FORCE_RESET();
    __HAL_RCC_NPU_RELEASE_RESET();
    npu_cache_init();
    /* USER CODE BEGIN 5 */
    /* USER CODE END 5 */
}

void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 6 */

    LL_ATON_RT_RetValues_t ll_aton_rt_ret;
    const LL_Buffer_InfoTypeDef *ibuffersInfos = NN_Interface_Default.input_buffers_info();
    const LL_Buffer_InfoTypeDef *obuffersInfos = NN_Interface_Default.output_buffers_info();

    /* Input buffer for NPU (f32, 196608 bytes) */
    float *input_f32 = (float *)LL_Buffer_addr_start(&ibuffersInfos[0]);
    uint32_t input_f32_len = ibuffersInfos->offset_end - ibuffersInfos->offset_start;

    /* Sort 4 output indices by ascending buffer size:
     *   idx[0] = proba_1 ( 384 floats, 1536 B)
     *   idx[1] = proba_0 ( 512 floats, 2048 B)
     *   idx[2] = boxes_1 (6144 floats, 24576 B)
     *   idx[3] = boxes_0 (8192 floats, 32768 B)
     */
    size_t out_idx[4];
    sort_output_indices(out_idx, obuffersInfos, 4);

    float *proba_1 = (float *)LL_Buffer_addr_start(&obuffersInfos[out_idx[0]]);
    float *proba_0 = (float *)LL_Buffer_addr_start(&obuffersInfos[out_idx[1]]);
    float *boxes_1 = (float *)LL_Buffer_addr_start(&obuffersInfos[out_idx[2]]);
    float *boxes_0 = (float *)LL_Buffer_addr_start(&obuffersInfos[out_idx[3]]);

    LL_ATON_RT_RuntimeInit();

    /* DCMIPP capture RGB888 to cam_buf, then convert uint8→f32 for NPU */
    if (HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0,
                                   (uint32_t)g_ai_cam_buf, DCMIPP_MODE_SNAPSHOT) != HAL_OK) {
        printf("ERROR: DCMIPP PIPE2 Start Failed!\r\n");
    }

    if (osSemaphoreAcquire(cam_frame_sem, pdMS_TO_TICKS(100)) != osOK) {
        printf("ERROR: Frame semaphore timeout!\r\n");
        LL_ATON_RT_RuntimeDeInit();
        return;
    }

    /* Convert uint8 RGB888 → float32 */
    uint32_t pixel_count = AI_BLAZEFACE_IMG_SIZE * AI_BLAZEFACE_IMG_SIZE * 3;
    for (uint32_t i = 0; i < pixel_count; i++) {
        input_f32[i] = (float)g_ai_cam_buf[i];
    }

    SCB_CleanDCache_by_Addr((uint32_t *)input_f32, input_f32_len);
    SCB_InvalidateDCache_by_Addr((uint32_t *)input_f32, input_f32_len);

    /* Run inference once */
    LL_ATON_RT_Init_Network(&NN_Instance_Default);
    do {
        ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);
        if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
            LL_ATON_OSAL_WFE();
        }
    } while (ll_aton_rt_ret != LL_ATON_RT_DONE);
    LL_ATON_RT_DeInit_Network(&NN_Instance_Default);

    /* Invalidate output cache so CPU sees fresh NPU results */
    for (int i = 0; i < 4; i++) {
        uint32_t addr = (uint32_t)LL_Buffer_addr_start(&obuffersInfos[i]);
        uint32_t len  = LL_Buffer_len(&obuffersInfos[i]);
        SCB_InvalidateDCache_by_Addr((uint32_t *)addr, len);
    }

    /* Decode: stride 0 (512 boxes) + stride 1 (384 boxes) */
    int total = blazeface_decode_stride(boxes, boxes_0, proba_0,
                                         g_Anchors_0, AI_BLAZEFACE_OUT_0_NB_BOXES,
                                         AI_BLAZEFACE_CONF_THRESHOLD);
    total += blazeface_decode_stride(boxes + total, boxes_1, proba_1,
                                      g_Anchors_1, AI_BLAZEFACE_OUT_1_NB_BOXES,
                                      AI_BLAZEFACE_CONF_THRESHOLD);

    /* NMS */
    int n_results = blazeface_nms(boxes, total, AI_BLAZEFACE_IOU_THRESHOLD,
                                   AI_BLAZEFACE_MAX_BOXES);

    /* Draw on LCD layer2: clear → draw detection boxes */
    hdma2d.Init.Mode          = DMA2D_R2M;
    hdma2d.Init.ColorMode     = DMA2D_OUTPUT_RGB888;
    hdma2d.Init.OutputOffset  = 0;
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_Start(&hdma2d, 0x00000000, (uint32_t)g_ltdc_layer2_framebuf, 800, 480);
    while (HAL_DMA2D_PollForTransfer(&hdma2d, 100) != HAL_OK) {}

    for (int i = 0; i < n_results; i++) {
        BlazeFaceBox *b = &boxes[i];

        /* Convert normalized coords [0,1] → LCD 800×480 */
        int x1 = (int)((b->x_center - b->width * 0.5f) * 800.0f);
        int y1 = (int)((b->y_center - b->height * 0.5f) * 480.0f);
        int x2 = (int)((b->x_center + b->width * 0.5f) * 800.0f);
        int y2 = (int)((b->y_center + b->height * 0.5f) * 480.0f);

        /* Clamp to screen */
        if (x1 < 0) x1 = 0; if (x1 > 799) x1 = 799;
        if (y1 < 0) y1 = 0; if (y1 > 479) y1 = 479;
        if (x2 < 0) x2 = 0; if (x2 > 799) x2 = 799;
        if (y2 < 0) y2 = 0; if (y2 > 479) y2 = 479;

        dma2d_draw_box_border(x1, y1, x2, y2, 0x00FF0000); // Red border
    }

    LL_ATON_RT_RuntimeDeInit();
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
