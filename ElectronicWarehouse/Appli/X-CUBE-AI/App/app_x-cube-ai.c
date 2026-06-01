
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body — BlazeFace inference + manual post-processing
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
#include "fd_blazeface_anchors.h"

/* USER CODE BEGIN includes */
/* USER CODE END includes */

/* Entry points --------------------------------------------------------------*/

LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(network_f)
uint8_t *buffer_in;

void set_clk_sleep_mode(void)
{
  /* Leave clocks enabled in Low Power modes */
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

    __HAL_RCC_RAMCFG_CLK_ENABLE();
    RAMCFG_HandleTypeDef hramcfg = {0};
    hramcfg.Instance =  RAMCFG_SRAM3_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    hramcfg.Instance =  RAMCFG_SRAM4_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    hramcfg.Instance =  RAMCFG_SRAM5_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    hramcfg.Instance =  RAMCFG_SRAM6_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    __HAL_RCC_XSPI1_CLK_SLEEP_ENABLE();
    __HAL_RCC_XSPI2_CLK_SLEEP_ENABLE();
    __HAL_RCC_NPU_CLK_SLEEP_ENABLE();
    __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE();
    __HAL_RCC_LTDC_CLK_SLEEP_ENABLE();
    __HAL_RCC_DMA2D_CLK_SLEEP_ENABLE();
    __HAL_RCC_DCMIPP_CLK_SLEEP_ENABLE();
    __HAL_RCC_CSI_CLK_SLEEP_ENABLE();

    LL_ATON_RT_RuntimeInit();
    LL_ATON_RT_Init_Network(&NN_Instance_network_f);

    /* Initialize input buffer pointer for external use (e.g. AI_task) */
    const LL_Buffer_InfoTypeDef *ib = NN_Interface_network_f.input_buffers_info();
    buffer_in = (uint8_t *)LL_Buffer_addr_start(&ib[0]);
    /* USER CODE END 5 */
}

/* -------------------------------------------------------------------------- */
/*                         Post-Processing Helpers                            */
/* -------------------------------------------------------------------------- */

static inline float sigmoid_f(float x)
{
    return 1.0f / (1.0f + expf(-x));
}

/* Compute IoU between two boxes (both in normalized coordinates [0,1]) */
static float box_iou(const ai_detection_t *a, const ai_detection_t *b)
{
    float ax1 = a->x_center - a->width  * 0.5f;
    float ay1 = a->y_center - a->height * 0.5f;
    float ax2 = a->x_center + a->width  * 0.5f;
    float ay2 = a->y_center + a->height * 0.5f;

    float bx1 = b->x_center - b->width  * 0.5f;
    float by1 = b->y_center - b->height * 0.5f;
    float bx2 = b->x_center + b->width  * 0.5f;
    float by2 = b->y_center + b->height * 0.5f;

    float ix1 = (ax1 > bx1) ? ax1 : bx1;
    float iy1 = (ay1 > by1) ? ay1 : by1;
    float ix2 = (ax2 < bx2) ? ax2 : bx2;
    float iy2 = (ay2 < by2) ? ay2 : by2;

    float iw = ix2 - ix1;
    float ih = iy2 - iy1;
    if (iw <= 0.0f || ih <= 0.0f) return 0.0f;

    float inter = iw * ih;
    float area_a = a->width * a->height;
    float area_b = b->width * b->height;
    float union_area = area_a + area_b - inter;
    if (union_area <= 0.0f) return 0.0f;

    return inter / union_area;
}

/* Simple descending sort by confidence (insertion sort, <1000 elements) */
static void sort_by_confidence(ai_detection_t *dets, uint32_t count)
{
    for (uint32_t i = 1; i < count; i++) {
        ai_detection_t key = dets[i];
        int32_t j = (int32_t)i - 1;
        while (j >= 0 && dets[j].confidence < key.confidence) {
            dets[j + 1] = dets[j];
            j--;
        }
        dets[j + 1] = key;
    }
}

/* -------------------------------------------------------------------------- */
/*                  BlazeFace Output Decoding + NMS                           */
/* -------------------------------------------------------------------------- */

static void ai_blazeface_postprocess(ai_result_t *result)
{
    const LL_Buffer_InfoTypeDef *ob = NN_Interface_network_f.output_buffers_info();

    /* Invalidate output buffer caches — NPU wrote to them via DMA */
    for (int i = 0; i < 4; i++) {
        uint32_t addr = LL_Buffer_addr_start(&ob[i]);
        uint32_t len  = LL_Buffer_len(&ob[i]);
        SCB_InvalidateDCache_by_Addr((uint32_t *)addr, len);
    }

    /* Identify the 4 output tensors by sorting indices by byte size.
       Expected: [0]=score_1(384), [1]=score_0(512), [2]=boxes_1, [3]=boxes_0 */
    uint8_t idx[4] = {0, 1, 2, 3};
    uint32_t sizes[4];
    for (int i = 0; i < 4; i++) {
        sizes[i] = LL_Buffer_len(&ob[i]);
    }
    /* bubble sort by size ascending */
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (sizes[idx[i]] > sizes[idx[j]]) {
                uint8_t t = idx[i]; idx[i] = idx[j]; idx[j] = t;
            }
        }
    }

    /* Now pair scores with boxes by element count:
       score[i] element count == boxes[j] element count / 16 */
    uint32_t score_count[2], box_count[2];
    for (int i = 0; i < 2; i++) {
        score_count[i] = sizes[idx[i]] / (ob[idx[i]].nbits / 8);
    }
    for (int i = 2; i < 4; i++) {
        box_count[i - 2] = (sizes[idx[i]] / (ob[idx[i]].nbits / 8)) / 16;
    }

    /* Match: find which box goes with which score */
    int box_for_score[2] = {-1, -1};
    for (int s = 0; s < 2; s++) {
        for (int b = 0; b < 2; b++) {
            if (score_count[s] == box_count[b]) {
                box_for_score[s] = b + 2;
                break;
            }
        }
    }

    /* DEBUG: print buffer info */
    printf("[DBG-BUF] nbits=%u\r\n", ob[0].nbits);
    for (int i = 0; i < 4; i++) {
        printf("[DBG-BUF] ob[%d] size=%lu type=%d nbits=%u\r\n",
               i, sizes[i], ob[i].type, ob[i].nbits);
    }
    printf("[DBG-BUF] sorted idx: %u %u %u %u\r\n", idx[0], idx[1], idx[2], idx[3]);
    printf("[DBG-BUF] scnt: %lu %lu  bcnt: %lu %lu\r\n",
           score_count[0], score_count[1], box_count[0], box_count[1]);

    /* DEBUG: print scale and offset for each output tensor */
    for (int i = 0; i < 4; i++) {
        const float *sc = ob[i].scale;
        const int16_t *zp = ob[i].offset;
        printf("[DBG-SCL] ob[%d] scale=%s zp=%s ",
               i,
               sc ? "SET" : "NULL",
               zp ? "SET" : "NULL");
        if (sc) printf("scale_v=%.6f ", *sc);
        if (zp) printf("zp_v=%d", *zp);
        printf("\r\n");
    }

    /* Static: too large for task stack (896 × 68 = 60KB). */
    static ai_detection_t candidates[896];
    uint32_t nb_candidates = 0;

    /* NPU model outputs values already in normalized [0,1] coordinates.
       Unlike TFLite which needs /128, the ONNX-converted model does not. */
    float inv_img = 1.0f;

    /* Decode two grids */
    for (int grid = 0; grid < 2; grid++) {
        int score_i  = idx[grid];           /* sorted score index */
        int box_i    = idx[box_for_score[grid]]; /* matched box index */
        uint32_t nb  = score_count[grid];        /* number of anchors */
        const float *anchors = (nb == 512) ? g_Anchors_0 : g_Anchors_1;

        const LL_Buffer_InfoTypeDef *s_info = &ob[score_i];
        const LL_Buffer_InfoTypeDef *b_info = &ob[box_i];

        uint8_t *score_buf = (uint8_t *)LL_Buffer_addr_start(s_info);
        uint8_t *box_buf   = (uint8_t *)LL_Buffer_addr_start(b_info);

        float s_scale = s_info->scale ? *s_info->scale : 1.0f;
        float b_scale = b_info->scale ? *b_info->scale : 1.0f;
        int16_t s_zp  = s_info->offset ? *s_info->offset : 0;
        int16_t b_zp  = b_info->offset ? *b_info->offset : 0;

        int score_is_float = (s_info->type == DataType_FLOAT);
        int box_is_float   = (b_info->type == DataType_FLOAT);
        int box_is_s8      = (b_info->type == DataType_INT8);
        int score_is_s8    = (s_info->type == DataType_INT8);

        /* DEBUG: print first 20 raw scores for this grid */
        printf("[DBG-SCO] grid=%d nb=%lu score_i=%d box_i=%d\r\n",
               grid, nb, score_i, box_i);
        printf("[DBG-SCO] raw: ");
        for (int di = 0; di < 20 && di < (int)nb; di++) {
            float rs;
            if (score_is_float) rs = ((float *)score_buf)[di];
            else if (score_is_s8) rs = (float)((int8_t *)score_buf)[di];
            else rs = (float)score_buf[di];
            printf("%.3f ", rs);
        }
        printf("\r\n");
        printf("[DBG-SCO] sig: ");
        for (int di = 0; di < 20 && di < (int)nb; di++) {
            float rs;
            if (score_is_float) rs = ((float *)score_buf)[di];
            else if (score_is_s8) rs = (float)((int8_t *)score_buf)[di];
            else rs = (float)score_buf[di];
            printf("%.3f ", sigmoid_f(rs));
        }
        printf("\r\n");

        for (uint32_t i = 0; i < nb; i++) {
            /* --- Decode score --- */
            float raw_score;
            if (score_is_float) {
                raw_score = ((float *)score_buf)[i];
            } else if (score_is_s8) {
                raw_score = ((int8_t *)score_buf)[i];
                raw_score = (raw_score - (float)s_zp) * s_scale;
            } else {
                raw_score = score_buf[i];
                raw_score = (raw_score - (float)s_zp) * s_scale;
            }

            float score = sigmoid_f(raw_score);
            if (score < AI_FD_CONF_THRESHOLD) continue;

            /* --- Decode box (16 values per anchor) --- */
            float raw[16];
            if (box_is_float) {
                float *fbox = &((float *)box_buf)[i * 16];
                for (int k = 0; k < 16; k++) raw[k] = fbox[k];
            } else if (box_is_s8) {
                int8_t *sbox = &((int8_t *)box_buf)[i * 16];
                for (int k = 0; k < 16; k++) {
                    raw[k] = ((float)sbox[k] - (float)b_zp) * b_scale;
                }
            } else {
                uint8_t *ubox = &box_buf[i * 16];
                for (int k = 0; k < 16; k++) {
                    raw[k] = ((float)ubox[k] - (float)b_zp) * b_scale;
                }
            }

            float ax = anchors[i * 2];
            float ay = anchors[i * 2 + 1];

            ai_detection_t det;
            det.x_center   = raw[0] * inv_img + ax;
            det.y_center   = raw[1] * inv_img + ay;
            det.width      = raw[2] * inv_img;
            det.height     = raw[3] * inv_img;
            det.confidence = score;

            /* DEBUG: print first detection above threshold */
            if (nb_candidates == 0) {
                printf("[DBG-DET] 1st det grid=%d i=%lu score=%.4f\r\n",
                       grid, i, score);
                printf("[DBG-DET] raw[0..3]: %.4f %.4f %.4f %.4f\r\n",
                       raw[0], raw[1], raw[2], raw[3]);
                printf("[DBG-DET] anchor: %.4f %.4f\r\n", ax, ay);
                printf("[DBG-DET] dec: cx=%.4f cy=%.4f w=%.4f h=%.4f\r\n",
                       det.x_center, det.y_center, det.width, det.height);
            }

            for (int k = 0; k < 6; k++) {
                det.keypoints[k][0] = raw[4 + k * 2]     * inv_img + ax;
                det.keypoints[k][1] = raw[4 + k * 2 + 1] * inv_img + ay;
            }

            candidates[nb_candidates++] = det;
        }
    }

    printf("[DBG-PP] total candidates before NMS: %lu\r\n", nb_candidates);

    /* NMS */
    if (nb_candidates == 0) {
        result->nb_detect = 0;
        return;
    }

    sort_by_confidence(candidates, nb_candidates);

    static uint8_t suppressed[896];
    memset(suppressed, 0, nb_candidates);

    result->nb_detect = 0;
    for (uint32_t i = 0; i < nb_candidates && result->nb_detect < AI_FD_MAX_DETECTIONS; i++) {
        if (suppressed[i]) continue;
        result->detections[result->nb_detect++] = candidates[i];
        for (uint32_t j = i + 1; j < nb_candidates; j++) {
            if (suppressed[j]) continue;
            if (box_iou(&candidates[i], &candidates[j]) > AI_FD_IOU_THRESHOLD) {
                suppressed[j] = 1;
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                           Inference Entry Point                            */
/* -------------------------------------------------------------------------- */

void MX_X_CUBE_AI_Process(ai_result_t *result)
{
    /* USER CODE BEGIN 6 */
    LL_ATON_RT_RetValues_t ll_aton_rt_ret;

    /* Perform single inference */
    LL_ATON_RT_Init_Network(&NN_Instance_network_f);
    do {
        ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_network_f);
        if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
            LL_ATON_OSAL_WFE();
        }
    } while (ll_aton_rt_ret != LL_ATON_RT_DONE);

    /* Post-process output buffers */
    ai_blazeface_postprocess(result);

    LL_ATON_RT_DeInit_Network(&NN_Instance_network_f);
    /* USER CODE END 6 */
}

#ifdef __cplusplus
}
#endif
