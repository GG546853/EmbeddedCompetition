
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

#include "fd_blazeface_anchors.h"
#include "network_fc.h"
#include "dma2d.h"

 void MX_X_CUBE_AI_Init(void);
 void set_clk_sleep_mode(void);

 LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(network_f)
 LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(network_fc)

 uint8_t *buffer_in;
 uint8_t *buffer_out;
 uint8_t *buffer_in_fc;
 uint8_t *buffer_out_fc;

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
 /*               Temporal EMA Smoothing (de-jitter)                           */
 /* -------------------------------------------------------------------------- */

static ai_detection_t smoothed_dets[AI_FD_MAX_DETECTIONS];
static uint32_t       smoothed_count;
static uint8_t        smoothing_initialized;

static void ai_detection_temporal_smooth(ai_result_t *result)
{
    if (!smoothing_initialized) {
        for (uint32_t i = 0; i < result->nb_detect; i++)
            smoothed_dets[i] = result->detections[i];
        smoothed_count = result->nb_detect;
        smoothing_initialized = 1;
        return;
    }

    ai_detection_t new_smoothed[AI_FD_MAX_DETECTIONS];
    uint32_t new_count = 0;
    uint8_t cur_matched[AI_FD_MAX_DETECTIONS] = {0};
    uint8_t prev_used[AI_FD_MAX_DETECTIONS] = {0};

    float alpha = AI_FD_EMA_ALPHA;

    /* Pass 1: match current detections to previous smoothed, apply EMA */
    for (uint32_t i = 0; i < result->nb_detect && new_count < AI_FD_MAX_DETECTIONS; i++) {
        int best_j = -1;
        float best_iou = 0.0f;
        for (uint32_t j = 0; j < smoothed_count; j++) {
            if (prev_used[j]) continue;
            float iou = box_iou(&result->detections[i], &smoothed_dets[j]);
            if (iou > best_iou) { best_iou = iou; best_j = (int)j; }
        }

        if (best_j >= 0 && best_iou > AI_FD_TRACK_IOU_THRESH) {
            ai_detection_t *cur  = &result->detections[i];
            ai_detection_t *prev = &smoothed_dets[best_j];
            ai_detection_t *out  = &new_smoothed[new_count];

            out->x_center   = alpha * cur->x_center   + (1.0f - alpha) * prev->x_center;
            out->y_center   = alpha * cur->y_center   + (1.0f - alpha) * prev->y_center;
            out->width      = alpha * cur->width      + (1.0f - alpha) * prev->width;
            out->height     = alpha * cur->height     + (1.0f - alpha) * prev->height;
            out->confidence = cur->confidence;

            for (int k = 0; k < 6; k++) {
                out->keypoints[k][0] = alpha * cur->keypoints[k][0]
                                     + (1.0f - alpha) * prev->keypoints[k][0];
                out->keypoints[k][1] = alpha * cur->keypoints[k][1]
                                     + (1.0f - alpha) * prev->keypoints[k][1];
            }

            prev_used[best_j] = 1;
            cur_matched[i] = 1;
            new_count++;
        }
    }

    /* Pass 2: add unmatched new detections (new faces) */
    for (uint32_t i = 0; i < result->nb_detect && new_count < AI_FD_MAX_DETECTIONS; i++) {
        if (!cur_matched[i]) {
            new_smoothed[new_count++] = result->detections[i];
        }
    }

    /* Write back */
    for (uint32_t i = 0; i < new_count; i++)
        smoothed_dets[i] = new_smoothed[i];
    smoothed_count = new_count;

    for (uint32_t i = 0; i < new_count; i++)
        result->detections[i] = smoothed_dets[i];
    result->nb_detect = new_count;
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

#if 0
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
#endif

     /* Static: too large for task stack (896 × 68 = 60KB). */
     static ai_detection_t candidates[896];
     uint32_t nb_candidates = 0;

     float inv_img = 1.0f / (float)AI_FD_IMG_SIZE;

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

#if 0
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
#endif

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
             det.width      = raw[2] * inv_img * AI_FD_BOX_SCALE_W;
             det.height     = raw[3] * inv_img * AI_FD_BOX_SCALE_H;
             det.confidence = score;

             /* reject boxes smaller than 10% of image */
             if (det.width < 0.10f || det.height < 0.10f) continue;

#if 0
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
#endif

             for (int k = 0; k < 6; k++) {
                 det.keypoints[k][0] = raw[4 + k * 2]     * inv_img + ax;
                 det.keypoints[k][1] = raw[4 + k * 2 + 1] * inv_img + ay;
             }

             candidates[nb_candidates++] = det;
         }
     }

#if 0
     printf("[DBG-PP] total candidates before NMS: %lu\r\n", nb_candidates);
#endif

     /* NMS */
     if (nb_candidates == 0) {
         result->nb_detect = 0;
         ai_detection_temporal_smooth(result);
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

     ai_detection_temporal_smooth(result);
 }

 /* -------------------------------------------------------------------------- */
 /*              Face Crop + Bilinear Resize to 112x112                        */
 /* -------------------------------------------------------------------------- */

 #define CROP_DISP_W  800
 #define CROP_DISP_H  480
 #define CROP_OUT_SZ  112
 #define CROP_MARGIN  0.3f   /* 30% expansion beyond detection box */

 /* Extract one RGB565 pixel's R/G/B as float [0,1] */
 static inline float r565(uint16_t p) { return ((p >> 11) & 0x1F) / 31.0f; }
 static inline float g565(uint16_t p) { return ((p >> 5)  & 0x3F) / 63.0f; }
 static inline float b565(uint16_t p) { return (p & 0x1F) / 31.0f; }

 void ai_crop_resize_face_112(uint16_t *src_fb, ai_detection_t *det, uint8_t *output)
 {
     extern DMA2D_HandleTypeDef hdma2d;
     extern uint8_t g_ltdc_layer2_framebuf[480 * 800 * 3];

     /* Map detection from [0,1] → display pixel coordinates */
     float dx = det->x_center * (float)CROP_DISP_W;
     float dy = det->y_center * (float)CROP_DISP_H;
     float dw = det->width    * (float)CROP_DISP_W;
     float dh = det->height   * (float)CROP_DISP_H;

     /* Square crop with margin, integer-clamped to image bounds */
     float size = (dw > dh ? dw : dh) * (1.0f + CROP_MARGIN);
     int crop_x0 = (int)(dx - size * 0.5f);
     int crop_y0 = (int)(dy - size * 0.5f);
     int crop_w  = (int)size;
     int crop_h  = (int)size;

     if (crop_x0 < 0) { crop_w += crop_x0; crop_x0 = 0; }
     if (crop_y0 < 0) { crop_h += crop_y0; crop_y0 = 0; }
     if (crop_x0 + crop_w > CROP_DISP_W) crop_w = CROP_DISP_W - crop_x0;
     if (crop_y0 + crop_h > CROP_DISP_H) crop_h = CROP_DISP_H - crop_y0;
     if (crop_w <= 0 || crop_h <= 0) return;

     printf("[CROP] dx=%.1f dy=%.1f size=%.1f x0=%d y0=%d w=%d h=%d\r\n",
            dx, dy, size, crop_x0, crop_y0, crop_w, crop_h);

     /* Borrow tail of layer2 framebuf (internal SRAM) for DMA2D temp buffer.
        CPU cannot read XSPI1 HyperRAM directly, but DMA2D can via its own bus master. */
     uint32_t temp_bytes = (uint32_t)crop_w * crop_h * 2;  /* RGB565 */
     uint16_t *temp_buf = (uint16_t *)(g_ltdc_layer2_framebuf
                         + sizeof(g_ltdc_layer2_framebuf) - temp_bytes);

     /* Step 1: DMA2D M2M_PFC copies crop region HyperRAM → internal SRAM.
        Source (HyperRAM) has line stride = CROP_DISP_W, destination is tightly packed. */
     hdma2d.Init.Mode          = DMA2D_M2M_PFC;
     hdma2d.Init.ColorMode     = DMA2D_OUTPUT_RGB565;
     hdma2d.Init.OutputOffset  = 0;
     hdma2d.Init.AlphaInverted = DMA2D_REGULAR_ALPHA;
     hdma2d.Init.RedBlueSwap   = DMA2D_RB_REGULAR;
     hdma2d.Init.BytesSwap     = DMA2D_BYTES_REGULAR;
     hdma2d.Init.LineOffsetMode = DMA2D_LOM_PIXELS;
     HAL_DMA2D_Init(&hdma2d);

     hdma2d.LayerCfg[0].InputOffset    = CROP_DISP_W - crop_w;
     hdma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_RGB565;
     hdma2d.LayerCfg[0].AlphaMode      = DMA2D_NO_MODIF_ALPHA;
     hdma2d.LayerCfg[0].InputAlpha     = 0xFF;
     hdma2d.LayerCfg[0].AlphaInverted  = DMA2D_REGULAR_ALPHA;
     hdma2d.LayerCfg[0].RedBlueSwap    = DMA2D_RB_REGULAR;
     HAL_DMA2D_ConfigLayer(&hdma2d, 0);

     HAL_DMA2D_Start(&hdma2d,
         (uint32_t)&src_fb[crop_y0 * CROP_DISP_W + crop_x0],
         (uint32_t)temp_buf,
         crop_w, crop_h);
     HAL_DMA2D_PollForTransfer(&hdma2d, 100);

     /* Invalidate D-Cache: DMA2D wrote to temp_buf, CPU will read it next */
     SCB_InvalidateDCache_by_Addr((uint32_t *)temp_buf, temp_bytes);

     printf("[CROP] DMA2D copy done, resizing %dx%d → 112×112\r\n", crop_w, crop_h);

     /* Step 2: Software bilinear resize from internal SRAM → 112×112 RGB888 */
     float inv_step = (float)(CROP_OUT_SZ - 1);  /* 127.0f */
     if (inv_step < 1.0f) inv_step = 1.0f;

     for (int oy = 0; oy < CROP_OUT_SZ; oy++) {
         float sy = ((float)oy / inv_step) * (float)(crop_h - 1);
         int y0 = (int)sy;
         int y1 = y0 + 1;
         float fy = sy - (float)y0;
         if (y1 >= crop_h) y1 = crop_h - 1;

         for (int ox = 0; ox < CROP_OUT_SZ; ox++) {
             float sx = ((float)ox / inv_step) * (float)(crop_w - 1);
             int x0 = (int)sx;
             int x1 = x0 + 1;
             float fx = sx - (float)x0;
             if (x1 >= crop_w) x1 = crop_w - 1;

             uint16_t p00 = temp_buf[y0 * crop_w + x0];
             uint16_t p10 = temp_buf[y0 * crop_w + x1];
             uint16_t p01 = temp_buf[y1 * crop_w + x0];
             uint16_t p11 = temp_buf[y1 * crop_w + x1];

             float w00 = (1.0f - fx) * (1.0f - fy);
             float w10 =        fx  * (1.0f - fy);
             float w01 = (1.0f - fx) *        fy;
             float w11 =        fx  *        fy;

             float r = w00 * r565(p00) + w10 * r565(p10)
                     + w01 * r565(p01) + w11 * r565(p11);
             float g = w00 * g565(p00) + w10 * g565(p10)
                     + w01 * g565(p01) + w11 * g565(p11);
             float b = w00 * b565(p00) + w10 * b565(p10)
                     + w01 * b565(p01) + w11 * b565(p11);

             int idx = (oy * CROP_OUT_SZ + ox) * 3;
             output[idx + 0] = (uint8_t)(r * 255.0f + 0.5f);
             output[idx + 1] = (uint8_t)(g * 255.0f + 0.5f);
             output[idx + 2] = (uint8_t)(b * 255.0f + 0.5f);
         }
     }
 }

 /* -------------------------------------------------------------------------- */
 /*           uint8 → int8 Quantization (match network_fc input scale)          */
 /* -------------------------------------------------------------------------- */

 /* network_fc input: int8, scale = 0.00784313772, zp = 0
    Camera pixel [0,255] → int8:
      s8 = round(u8 / 255.0 / 0.00784313772)  */
 #define FC_INPUT_SCALE  0.00784313772f

 static const int8_t quant_u8_to_s8_lut[256] = {
       0,   0,   1,   1,   2,   2,   3,   3,   4,   4,   5,   5,   6,   6,   7,   7,
       8,   8,   9,   9,  10,  10,  11,  11,  12,  12,  13,  13,  14,  14,  15,  15,
      16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21,  21,  22,  22,  23,  23,
      24,  24,  25,  25,  26,  26,  27,  27,  28,  28,  29,  29,  30,  30,  31,  31,
      32,  32,  33,  33,  34,  34,  35,  35,  36,  36,  37,  37,  38,  38,  39,  39,
      40,  40,  41,  41,  42,  42,  43,  43,  44,  44,  45,  45,  46,  46,  47,  47,
      48,  48,  49,  49,  50,  50,  51,  51,  52,  52,  53,  53,  54,  54,  55,  55,
      56,  56,  57,  57,  58,  58,  59,  59,  60,  60,  61,  61,  62,  62,  63,  63,
      64,  64,  65,  65,  66,  66,  67,  67,  68,  68,  69,  69,  70,  70,  71,  71,
      72,  72,  73,  73,  74,  74,  75,  75,  76,  76,  77,  77,  78,  78,  79,  79,
      80,  80,  81,  81,  82,  82,  83,  83,  84,  84,  85,  85,  86,  86,  87,  87,
      88,  88,  89,  89,  90,  90,  91,  91,  92,  92,  93,  93,  94,  94,  95,  95,
      96,  96,  97,  97,  98,  98,  99,  99, 100, 100, 101, 101, 102, 102, 103, 103,
     104, 104, 105, 105, 106, 106, 107, 107, 108, 108, 109, 109, 110, 110, 111, 111,
     112, 112, 113, 113, 114, 114, 115, 115, 116, 116, 117, 117, 118, 118, 119, 119,
     120, 120, 121, 121, 122, 122, 123, 123, 124, 124, 125, 125, 126, 126, 127, 127,
 };

 void quantize_u8_to_s8(const uint8_t *u8, int8_t *s8, uint32_t count)
 {
     for (uint32_t i = 0; i < count; i++) {
         s8[i] = quant_u8_to_s8_lut[u8[i]];
     }
 }

 /* -------------------------------------------------------------------------- */
 /*                        Face Gallery (Module 4)                              */
 /* -------------------------------------------------------------------------- */

 static face_entry_t face_gallery[FACE_GALLERY_MAX];
 static uint32_t     face_gallery_count;

 /* L2-normalize a vector in-place */
 static void l2_normalize(float *v, int dim)
 {
     float sum_sq = 0.0f;
     for (int i = 0; i < dim; i++) {
         sum_sq += v[i] * v[i];
     }
     if (sum_sq < 1e-30f) return;
     float inv_norm = 1.0f / sqrtf(sum_sq);
     for (int i = 0; i < dim; i++) {
         v[i] *= inv_norm;
     }
 }

 int ai_face_enroll(const float *embedding, const char *name)
 {
     if (face_gallery_count >= FACE_GALLERY_MAX) return -1;
     /* Store L2-normalised embedding */
     memcpy(face_gallery[face_gallery_count].embedding,
            embedding, FACE_EMBEDDING_DIM * sizeof(float));
     l2_normalize(face_gallery[face_gallery_count].embedding, FACE_EMBEDDING_DIM);
     strncpy(face_gallery[face_gallery_count].name, name, FACE_NAME_MAX - 1);
     face_gallery[face_gallery_count].name[FACE_NAME_MAX - 1] = '\0';
     face_gallery_count++;
     return 0;
 }

 int ai_face_identify(const float *embedding, char *name_out, float *dist_out)
 {
     if (face_gallery_count == 0) { if (dist_out) *dist_out = 0.0f; return -1; }

     /* L2-normalise the query embedding */
     float query[FACE_EMBEDDING_DIM];
     memcpy(query, embedding, sizeof(query));
     l2_normalize(query, FACE_EMBEDDING_DIM);

     float best_dist   = 1e9f;
     float second_dist = 1e9f;
     int   best_idx    = -1;

     for (uint32_t i = 0; i < face_gallery_count; i++) {
         float sum_sq = 0.0f;
         for (int d = 0; d < FACE_EMBEDDING_DIM; d++) {
             float diff = query[d] - face_gallery[i].embedding[d];
             sum_sq += diff * diff;
         }
         float dist = sqrtf(sum_sq);
         if (dist < best_dist) {
             second_dist = best_dist;
             best_dist   = dist;
             best_idx    = (int)i;
         } else if (dist < second_dist) {
             second_dist = dist;
         }
     }

     if (best_idx < 0) { if (dist_out) *dist_out = 0.0f; return -1; }

     /* Distance ratio check: only when it's not already an obvious match.
        If best_dist is very small, skip ratio check — we're confident. */
     if (best_dist > FACE_CONFIDENT_DIST && face_gallery_count >= 2) {
         float ratio = best_dist / second_dist;
         if (ratio > FACE_RATIO_THRESHOLD) {
             if (dist_out) *dist_out = best_dist;
             printf("[REID] Rejected: ratio=%.3f (best=%.3f, second=%.3f, threshold=%.2f)\r\n",
                    ratio, best_dist, second_dist, (double)FACE_RATIO_THRESHOLD);
             return -1;
         }
     }

     if (best_dist < FACE_MATCH_THRESHOLD) {
         if (name_out) strncpy(name_out, face_gallery[best_idx].name, FACE_NAME_MAX);
         if (dist_out) *dist_out = best_dist;
         return best_idx;
     }
     if (dist_out) *dist_out = best_dist;
     return -1;
 }

 void ai_face_gallery_print(void)
 {
     printf("[GALLERY] %lu faces registered:\r\n", face_gallery_count);
     for (uint32_t i = 0; i < face_gallery_count; i++) {
         printf("  [%lu] %s\r\n", i, face_gallery[i].name);
     }
 }

 /* -------------------------------------------------------------------------- */
 /*                    network_fc Inference Wrapper                             */
 /* -------------------------------------------------------------------------- */

 void ai_face_reid_run(float *embedding_out)
 {
     LL_ATON_RT_RetValues_t ret;

     printf("[FC] Init start...\r\n");
     LL_ATON_RT_Init_Network(&NN_Instance_network_fc);
     printf("[FC] Init done, running epochs...\r\n");

     int epoch_cnt = 0;
     do {
         ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_network_fc);
         if (ret == LL_ATON_RT_WFE) {
             LL_ATON_OSAL_WFE();
         }
         epoch_cnt++;
     } while (ret != LL_ATON_RT_DONE);
     printf("[FC] Epochs done (%d), decoding output...\r\n", epoch_cnt);

     /* Invalidate output cache */
     const LL_Buffer_InfoTypeDef *ob_fc = NN_Interface_network_fc.output_buffers_info();
     SCB_InvalidateDCache_by_Addr(
         (uint32_t *)LL_Buffer_addr_start(&ob_fc[0]),
         LL_Buffer_len(&ob_fc[0]));

     /* Decode output: handle both float32 and int8 */
     if (ob_fc[0].type == DataType_FLOAT) {
         float *src = (float *)LL_Buffer_addr_start(&ob_fc[0]);
         uint32_t n = LL_Buffer_len(&ob_fc[0]) / sizeof(float);
         for (uint32_t i = 0; i < FACE_EMBEDDING_DIM && i < n; i++)
             embedding_out[i] = src[i];
     } else {
         /* int8 → dequantize */
         int8_t *src = (int8_t *)LL_Buffer_addr_start(&ob_fc[0]);
         float scale = ob_fc[0].scale ? *ob_fc[0].scale : 0.0106163137f;
         int16_t zp  = ob_fc[0].offset ? *ob_fc[0].offset : 0;
         uint32_t n = LL_Buffer_len(&ob_fc[0]);
         for (uint32_t i = 0; i < FACE_EMBEDDING_DIM && i < n; i++)
             embedding_out[i] = ((float)src[i] - (float)zp) * scale;
     }

     LL_ATON_RT_DeInit_Network(&NN_Instance_network_fc);
     printf("[FC] DeInit done\r\n");
 }

 void MX_X_CUBE_AI_Process_User(ai_result_t *result)
 {

     LL_ATON_RT_RetValues_t ll_aton_rt_ret;


     LL_ATON_RT_Init_Network(&NN_Instance_network_f);
     do {
         ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_network_f);
         if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
             LL_ATON_OSAL_WFE();
         }
     } while (ll_aton_rt_ret != LL_ATON_RT_DONE);


     ai_blazeface_postprocess(result);

     LL_ATON_RT_DeInit_Network(&NN_Instance_network_f);

 }


/* USER CODE END includes */

/* Entry points --------------------------------------------------------------*/



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

    __HAL_RCC_RAMCFG_CLK_ENABLE();
    RAMCFG_HandleTypeDef hramcfg = {0};
    hramcfg.Instance =  RAMCFG_SRAM2_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
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
    LL_ATON_RT_DeInit_Network(&NN_Instance_network_f);

    /* Initialize network_fc buffer pointers (brief init to discover addresses).
       Must DeInit network_f first — the two models share npuRAM4. */
    LL_ATON_RT_Init_Network(&NN_Instance_network_fc);
    const LL_Buffer_InfoTypeDef *ib_fc = NN_Interface_network_fc.input_buffers_info();
    const LL_Buffer_InfoTypeDef *ob_fc = NN_Interface_network_fc.output_buffers_info();
    buffer_in_fc  = (uint8_t *)LL_Buffer_addr_start(&ib_fc[0]);
    buffer_out_fc = (uint8_t *)LL_Buffer_addr_start(&ob_fc[0]);
    LL_ATON_RT_DeInit_Network(&NN_Instance_network_fc);
    /* USER CODE END 5 */
}

void MX_X_CUBE_AI_Process(void)
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


     LL_ATON_RT_DeInit_Network(&NN_Instance_network_f);
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
