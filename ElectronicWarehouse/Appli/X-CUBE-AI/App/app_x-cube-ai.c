
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
#include "profiling.h"

 void MX_X_CUBE_AI_Init(void);
 void set_clk_sleep_mode(void);

 LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(network_f)
 LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(network_fc)

 uint8_t *buffer_in;
 uint8_t *buffer_out;
 uint8_t *buffer_in_fc;
 uint8_t *buffer_out_fc;

 /* -------------------------------------------------------------------------- */
 /*                     CRC32 for Weight Integrity Check                        */
 /* -------------------------------------------------------------------------- */

 static uint32_t crc32_table[256];
 static int crc32_table_ready;

 static void crc32_init(void)
 {
     if (crc32_table_ready) return;
     for (uint32_t i = 0; i < 256; i++) {
         uint32_t crc = i;
         for (int j = 0; j < 8; j++)
             crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320UL : 0);
         crc32_table[i] = crc;
     }
     crc32_table_ready = 1;
 }

 static uint32_t crc32_update(uint32_t crc, const uint8_t *data, uint32_t len)
 {
     crc = ~crc;
     for (uint32_t i = 0; i < len; i++)
         crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
     return ~crc;
 }

 uint32_t fc_weight_crc32(void)
 {
     crc32_init();
     return crc32_update(0, (const uint8_t *)0x71200000, 1156289);
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
             det.x_center   = raw[0] * inv_img + ax + AI_FD_X_BIAS;
             det.y_center   = raw[1] * inv_img + ay;
             det.width      = raw[2] * inv_img * AI_FD_BOX_SCALE_W;
             det.height     = raw[3] * inv_img * AI_FD_BOX_SCALE_H;
             det.confidence = score;

             /* reject boxes smaller than 10% of image — temporarily disabled */
             // if (det.width < 0.30f || det.height < 0.40f) continue;

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
                 det.keypoints[k][0] = raw[4 + k * 2]     * inv_img + ax + AI_FD_X_BIAS;
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
 #define CROP_MARGIN       0.0f
#define EYE_CROP_FACTOR   2.4f   /* crop size = eye_dist * factor */
#define VERT_OFFSET        0.1f  /* center offset below eye line, as fraction of eye_dist */
#define BOX_PAD_RATIO      0.1f  /* detection box padding for direct-resize mode */

/* Crop mode: 0 = eye-distance square crop, 1 = detection-box direct resize */
#define CROP_MODE 1

#define CROP_TEMP_MAX_PX 480
#define CROP_TEMP_SIZE  (CROP_TEMP_MAX_PX * CROP_TEMP_MAX_PX * 3)

static uint8_t crop_temp_buf[CROP_TEMP_SIZE]
    __attribute__((section(".crop_buf"), aligned(32)));

 void ai_crop_resize_face_112(uint8_t *src_fb, ai_detection_t *det, uint8_t *output)
 {
     extern DMA2D_HandleTypeDef hdma2d;

#if CROP_MODE == 0
     /* --- Eye-distance square crop --- */
     float le_x = det->keypoints[0][0] * 600.0f + 100.0f;
     float le_y = det->keypoints[0][1] * (float)CROP_DISP_H;
     float re_x = det->keypoints[1][0] * 600.0f + 100.0f;
     float re_y = det->keypoints[1][1] * (float)CROP_DISP_H;

     float eye_dx = re_x - le_x;
     float eye_dy = re_y - le_y;
     float eye_dist = sqrtf(eye_dx * eye_dx + eye_dy * eye_dy);

     float dx, dy, size;

     if (eye_dist > 15.0f) {
         dx   = (le_x + re_x) * 0.5f;
         dy   = (le_y + re_y) * 0.5f + eye_dist * VERT_OFFSET;
         size = eye_dist * EYE_CROP_FACTOR;
     } else {
         dx   = det->x_center * 600.0f + 100.0f;
         dy   = det->y_center * (float)CROP_DISP_H;
         float dw = det->width    * 600.0f;
         float dh = det->height   * (float)CROP_DISP_H;
         size = (dw > dh ? dw : dh) * (1.0f + CROP_MARGIN);
     }

     int crop_x0 = (int)(dx - size * 0.5f);
     int crop_y0 = (int)(dy - size * 0.5f);
     int crop_w  = (int)size;
     int crop_h  = (int)size;

#elif CROP_MODE == 1
     /* --- Detection-box direct resize with padding --- */
     float bx = det->x_center * 600.0f + 100.0f;
     float by = det->y_center * (float)CROP_DISP_H;
     float bw = det->width  * 600.0f;
     float bh = det->height * (float)CROP_DISP_H;

     float pad_w = bw * BOX_PAD_RATIO;
     float pad_h = bh * BOX_PAD_RATIO;

     int crop_x0 = (int)(bx - bw * 0.5f - pad_w);
     int crop_y0 = (int)(by - bh * 0.5f - pad_h);
     int crop_w  = (int)(bw + pad_w * 2.0f);
     int crop_h  = (int)(bh + pad_h * 2.0f);
#endif

     if (crop_x0 < 0) { crop_w += crop_x0; crop_x0 = 0; }
     if (crop_y0 < 0) { crop_h += crop_y0; crop_y0 = 0; }
     if (crop_x0 + crop_w > CROP_DISP_W) crop_w = CROP_DISP_W - crop_x0;
     if (crop_y0 + crop_h > CROP_DISP_H) crop_h = CROP_DISP_H - crop_y0;
     if (crop_w <= 0 || crop_h <= 0) return;

//     printf("[CROP] eye_dist=%.1f dx=%.1f dy=%.1f size=%.1f x0=%d y0=%d w=%d h=%d\r\n",
//            eye_dist, dx, dy, size, crop_x0, crop_y0, crop_w, crop_h);

     /* Clamp crop to temp buffer capacity */
     if (crop_w > CROP_TEMP_MAX_PX) crop_w = CROP_TEMP_MAX_PX;
     if (crop_h > CROP_TEMP_MAX_PX) crop_h = CROP_TEMP_MAX_PX;

     /* Step 1: CPU copy crop region (RGB888) src_fb → crop_temp_buf.
        Source has line stride = CROP_DISP_W * 3 bytes. Dest is tightly packed.
        Using CPU copy to rule out DMA2D M2M_PFC as the horizontal-flattening culprit. */
     uint32_t temp_bytes = (uint32_t)crop_w * crop_h * 3;

     for (int ry = 0; ry < crop_h; ry++) {
         uint8_t *src_row = src_fb + ((crop_y0 + ry) * CROP_DISP_W + crop_x0) * 3;
         uint8_t *dst_row = crop_temp_buf + ry * crop_w * 3;
         for (int rx = 0; rx < crop_w; rx++) {
             /* PIPE1 outputs BGR, swap to RGB here */
             dst_row[rx * 3 + 0] = src_row[rx * 3 + 2];  /* R ← src B */
             dst_row[rx * 3 + 1] = src_row[rx * 3 + 1];  /* G ← src G */
             dst_row[rx * 3 + 2] = src_row[rx * 3 + 0];  /* B ← src R */
         }
     }

     /* CPU wrote crop_temp_buf — flush DCache so bilinear resize (CPU) sees it.
        No-op if DCache is disabled. */
     SCB_CleanDCache_by_Addr((uint32_t *)crop_temp_buf, temp_bytes);

     /* Step 2: Software bilinear resize crop_temp_buf (RGB888) → 112×112 RGB888 */
     float inv_step = (float)(CROP_OUT_SZ - 1);
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

             uint8_t *p00 = &crop_temp_buf[(y0 * crop_w + x0) * 3];
             uint8_t *p10 = &crop_temp_buf[(y0 * crop_w + x1) * 3];
             uint8_t *p01 = &crop_temp_buf[(y1 * crop_w + x0) * 3];
             uint8_t *p11 = &crop_temp_buf[(y1 * crop_w + x1) * 3];

             float w00 = (1.0f - fx) * (1.0f - fy);
             float w10 =        fx  * (1.0f - fy);
             float w01 = (1.0f - fx) *        fy;
             float w11 =        fx  *        fy;

             float r = w00 * p00[0] + w10 * p10[0]
                     + w01 * p01[0] + w11 * p11[0];
             float g = w00 * p00[1] + w10 * p10[1]
                     + w01 * p01[1] + w11 * p11[1];
             float b = w00 * p00[2] + w10 * p10[2]
                     + w01 * p01[2] + w11 * p11[2];

             int idx = (oy * CROP_OUT_SZ + ox) * 3;
             output[idx + 0] = (uint8_t)(r + 0.5f);
             output[idx + 1] = (uint8_t)(g + 0.5f);
             output[idx + 2] = (uint8_t)(b + 0.5f);
         }
     }
 }

 /* -------------------------------------------------------------------------- */
 /*           uint8 → int8 Quantization (match network_fc input scale)          */
 /* -------------------------------------------------------------------------- */

 /* network_fc input: int8, scale = 0.007843138, zp = 0
    Model was trained with [-1, 1] normalization (MobileFaceNet standard):
      float_norm = u8 / 127.5 - 1.0        → [-1, 1]
      s8 = round(float_norm / 0.007843138) → int8 [-128, 127]
    Combined:
      s8 = round((u8/127.5 - 1.0) / 0.007843138)
         = round(u8 - 127.5)
         ≈ (int8_t)((int16_t)u8 - 128)                              */

 void quantize_u8_to_s8(const uint8_t *u8, int8_t *s8, uint32_t count)
 {
     for (uint32_t i = 0; i < count; i++) {
         int16_t val = (int16_t)u8[i] - 128;
         /* odd u8 → fractional 0.5 → round up (clamp to INT8_MAX) */
         if ((u8[i] & 1) && val < 127) {
             s8[i] = (int8_t)(val + 1);
         } else {
             s8[i] = (int8_t)val;
         }
     }
 }

 /* -------------------------------------------------------------------------- */
 /*                        Face Gallery (Module 4)                              */
 /* -------------------------------------------------------------------------- */

 /*
  * Default gallery entry: zs_pc — PC-side enrolled embedding (L2-normalized).
  * Imported from C:\Users\96022\Desktop\FACE_INT8\face_db.json
  * PC verification: same-person cos≈0.85, stranger cos≈0.75
  */
 static face_entry_t face_gallery[FACE_GALLERY_MAX] = {
     { .embedding = {
         0.0464f, 0.0166f, 0.1019f, -0.0212f, -0.0479f, 0.1120f, 0.0418f, 0.0946f,
         0.1829f, 0.0130f, -0.0099f, -0.0512f, 0.0343f, 0.0820f, 0.0820f, 0.1439f,
        -0.0768f, -0.0297f, 0.0137f, -0.0488f, 0.0915f, -0.0842f, -0.1286f, -0.1661f,
        -0.0307f, 0.0029f, -0.0217f, -0.1306f, 0.0494f, -0.0335f, -0.0032f, 0.0409f,
        -0.1004f, -0.1386f, -0.0100f, -0.0845f, 0.0070f, -0.1699f, 0.0612f, -0.0660f,
        -0.0838f, 0.0268f, 0.0245f, -0.1623f, 0.0739f, -0.1622f, 0.0502f, -0.0036f,
        -0.1305f, 0.0591f, -0.2211f, -0.1136f, -0.0074f, -0.0786f, 0.0303f, -0.0121f,
         0.0348f, -0.1563f, 0.0452f, 0.0193f, 0.1168f, -0.1483f, -0.0708f, -0.0188f,
        -0.1677f, 0.0642f, -0.0940f, -0.1703f, 0.0584f, -0.0398f, -0.0504f, -0.1083f,
         0.0938f, 0.0203f, -0.0705f, 0.0766f, -0.0199f, 0.0076f, -0.0920f, -0.0420f,
        -0.0092f, -0.1035f, 0.2035f, -0.0164f, -0.0038f, -0.0689f, -0.0034f, 0.1293f,
         0.0797f, 0.0706f, -0.0140f, 0.0325f, -0.0191f, 0.0812f, -0.0477f, 0.0916f,
         0.0024f, 0.1412f, -0.0363f, -0.1156f, -0.0096f, 0.0045f, 0.0713f, 0.0798f,
         0.1551f, 0.0202f, 0.1482f, -0.0542f, 0.0933f, 0.0168f, 0.0152f, -0.1113f,
         0.1172f, -0.0491f, -0.0257f, -0.1647f, 0.0484f, 0.0355f, -0.0784f, -0.1046f,
        -0.1146f, 0.0382f, -0.0775f, 0.1954f, -0.0840f, 0.0698f, 0.0866f, -0.1070f
       },
       .name = "zs_pc" },
 };
 static uint32_t     face_gallery_count = 1;

 /* ---------- Multi-shot enrollment state ---------- */

 static int      enroll_active;
 static char     enroll_name[FACE_NAME_MAX];
 static float    enroll_accum[FACE_EMBEDDING_DIM];
 static int      enroll_count;

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
     /* Check for duplicate: update existing entry with same name */
     for (uint32_t i = 0; i < face_gallery_count; i++) {
         if (strncmp(face_gallery[i].name, name, FACE_NAME_MAX) == 0) {
             memcpy(face_gallery[i].embedding,
                    embedding, FACE_EMBEDDING_DIM * sizeof(float));
             l2_normalize(face_gallery[i].embedding, FACE_EMBEDDING_DIM);
             printf("[REID] Updated '%s' (slot %lu)\r\n", name, i);
             return (int)i;
         }
     }
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

 /*
  * Multi-shot enrollment: accumulate N normalized embeddings, then
  * store the averaged (and re-normalised) result.  Call once per frame
  * while reid_pending == REID_REGISTER; the caller keeps the state
  * alive until remaining == 0.
  *
  * Returns remaining samples needed (0 = done and stored).
  */
 int ai_face_enroll_multi(const float *embedding, const char *name)
 {
     /* Reset on first call or name change */
     if (!enroll_active || strncmp(enroll_name, name, FACE_NAME_MAX) != 0) {
         enroll_active = 1;
         strncpy(enroll_name, name, FACE_NAME_MAX - 1);
         enroll_name[FACE_NAME_MAX - 1] = '\0';
         memset(enroll_accum, 0, sizeof(enroll_accum));
         enroll_count  = 0;
     }

     /* Normalise this embedding and accumulate */
     float norm_emb[FACE_EMBEDDING_DIM];
     memcpy(norm_emb, embedding, sizeof(norm_emb));
     l2_normalize(norm_emb, FACE_EMBEDDING_DIM);

     /* Quality gate: reject if too different from running average (skip 1st sample) */
     if (enroll_count > 0) {
         float avg[FACE_EMBEDDING_DIM];
         for (int i = 0; i < FACE_EMBEDDING_DIM; i++)
             avg[i] = enroll_accum[i] / (float)enroll_count;
         l2_normalize(avg, FACE_EMBEDDING_DIM);
         float cosim = 0.0f;
         for (int i = 0; i < FACE_EMBEDDING_DIM; i++)
             cosim += norm_emb[i] * avg[i];
         if (cosim < ENROLL_QUALITY_COSIM) {
             printf("[REID] Sample REJECTED (cos=%.4f < %.2f), keep still & re-send 's'\r\n",
                    cosim, ENROLL_QUALITY_COSIM);
             return ENROLL_QUALITY_REJECT;
         }
     }

     for (int i = 0; i < FACE_EMBEDDING_DIM; i++) {
         enroll_accum[i] += norm_emb[i];
     }
     enroll_count++;

     printf("[REID] Enroll sample %d/%d for '%s'\r\n",
            enroll_count, FACE_ENROLL_SAMPLES, enroll_name);

     if (enroll_count >= FACE_ENROLL_SAMPLES) {
         /* Average and re-normalise */
         for (int i = 0; i < FACE_EMBEDDING_DIM; i++) {
             enroll_accum[i] /= (float)enroll_count;
         }
         l2_normalize(enroll_accum, FACE_EMBEDDING_DIM);

         /* Store in gallery (reuse single-shot logic) */
         int duplicate = 0;
         for (uint32_t i = 0; i < face_gallery_count; i++) {
             if (strncmp(face_gallery[i].name, enroll_name, FACE_NAME_MAX) == 0) {
                 memcpy(face_gallery[i].embedding, enroll_accum,
                        FACE_EMBEDDING_DIM * sizeof(float));
                 printf("[REID] Updated '%s' (slot %lu)\r\n", enroll_name, i);
                 duplicate = 1;
                 break;
             }
         }
         if (!duplicate) {
             if (face_gallery_count >= FACE_GALLERY_MAX) {
                 printf("[REID] Gallery full, cannot enroll '%s'\r\n", enroll_name);
             } else {
                 memcpy(face_gallery[face_gallery_count].embedding, enroll_accum,
                        FACE_EMBEDDING_DIM * sizeof(float));
                 strncpy(face_gallery[face_gallery_count].name, enroll_name,
                         FACE_NAME_MAX - 1);
                 face_gallery[face_gallery_count].name[FACE_NAME_MAX - 1] = '\0';
                 face_gallery_count++;
                 printf("[REID] Enrolled '%s' into slot %lu\r\n",
                        enroll_name, face_gallery_count - 1);
             }
         }

         enroll_active = 0;
         return 0;
     }

     return FACE_ENROLL_SAMPLES - enroll_count;
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
     int   second_idx  = -1;

     for (uint32_t i = 0; i < face_gallery_count; i++) {
         float sum_sq = 0.0f;
         for (int d = 0; d < FACE_EMBEDDING_DIM; d++) {
             float diff = query[d] - face_gallery[i].embedding[d];
             sum_sq += diff * diff;
         }
         float dist = sqrtf(sum_sq);
         if (dist < best_dist) {
             second_dist = best_dist;
             second_idx  = best_idx;
             best_dist   = dist;
             best_idx    = (int)i;
         } else if (dist < second_dist) {
             second_dist = dist;
             second_idx  = (int)i;
         }
     }

     if (best_idx < 0) { if (dist_out) *dist_out = 0.0f; return -1; }

     /* Distance ratio check: only when there are 2+ different people,
        and it's not already an obvious match. */
     if (best_dist > FACE_CONFIDENT_DIST && face_gallery_count >= 2
         && second_idx >= 0
         && strncmp(face_gallery[best_idx].name,
                    face_gallery[second_idx].name, FACE_NAME_MAX) != 0) {
         float ratio = best_dist / second_dist;
         if (ratio > FACE_RATIO_THRESHOLD) {
             if (dist_out) *dist_out = best_dist;
             printf("[REID] Rejected: ratio=%.3f dist=%.3f (threshold=%.2f)\r\n",
                    ratio, best_dist, (double)FACE_MATCH_THRESHOLD);
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
/*                       1:1 Verification                                     */
/* -------------------------------------------------------------------------- */

int ai_face_select(const char *name, face_entry_t *out_entry)
{
    for (uint32_t i = 0; i < face_gallery_count; i++) {
        if (strncmp(face_gallery[i].name, name, FACE_NAME_MAX) == 0) {
            memcpy(out_entry, &face_gallery[i], sizeof(face_entry_t));
            return 0;
        }
    }
    return -1;
}

int ai_face_verify(const float *embedding, const face_entry_t *target, float *dist_out)
{
    float query[FACE_EMBEDDING_DIM];
    memcpy(query, embedding, sizeof(query));
    l2_normalize(query, FACE_EMBEDDING_DIM);

    float sum_sq = 0.0f;
    for (int d = 0; d < FACE_EMBEDDING_DIM; d++) {
        float diff = query[d] - target->embedding[d];
        sum_sq += diff * diff;
    }
    float dist = sqrtf(sum_sq);

    if (dist_out) *dist_out = dist;

    if (dist < FACE_VERIFY_THRESHOLD) return 1;
    return 0;
}

void ai_face_gallery_export(face_entry_t *out_entries, uint32_t *out_count)
{
    memcpy(out_entries, face_gallery, sizeof(face_gallery));
    *out_count = face_gallery_count;
}

void ai_face_gallery_import(const face_entry_t *in_entries, uint32_t in_count)
{
    memcpy(face_gallery, in_entries, sizeof(face_gallery));
    if (in_count <= FACE_GALLERY_MAX) {
        face_gallery_count = in_count;
    } else {
        face_gallery_count = FACE_GALLERY_MAX;
    }
}

void ai_face_gallery_set_name(int idx, const char *name)
{
    if (idx < 0 || idx >= (int)face_gallery_count) return;
    strncpy(face_gallery[idx].name, name, FACE_NAME_MAX - 1);
    face_gallery[idx].name[FACE_NAME_MAX - 1] = '\0';
}

uint32_t ai_face_gallery_get_count(void)
{
    return face_gallery_count;
}

 /* -------------------------------------------------------------------------- */
 /*                    network_fc Inference Wrapper                             */
 /* -------------------------------------------------------------------------- */

 /* Structural epoch counters: called by the LL_ATON runtime before each epoch
    starts. Only counts, never blocks or prints (runs in the inference context). */
 static void prof_epoch_cb(LL_ATON_RT_Callbacktype_t ctype,
                           const NN_Instance_TypeDef *nn_instance,
                           const EpochBlock_ItemTypeDef *epoch_block)
 {
     if (ctype != LL_ATON_RT_Callbacktype_PRE_START || epoch_block == NULL) return;

     prof_net_t n;
     if (nn_instance == &NN_Instance_network_f) {
         n = PROF_NET_F;
     } else if (nn_instance == &NN_Instance_network_fc) {
         n = PROF_NET_FC;
     } else {
         return;
     }
     profiling_epoch(n,
                     EpochBlock_IsEpochPureSW(epoch_block),
                     EpochBlock_IsEpochHybrid(epoch_block),
                     EpochBlock_IsEpochBlob(epoch_block),
                     EpochBlock_IsEpochInternal(epoch_block));
 }

 void ai_face_reid_run(float *embedding_out)
 {
     LL_ATON_RT_RetValues_t ret;
     uint32_t wall0 = profiling_tick_now();
     uint64_t t;

     t = profiling_now();
     LL_ATON_RT_Init_Network(&NN_Instance_network_fc);
     profiling_npu_init(PROF_NET_FC, profiling_now() - t);

     do {
         t = profiling_now();
         ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_network_fc);
         profiling_npu_runblock(PROF_NET_FC, profiling_now() - t);
         if (ret == LL_ATON_RT_WFE) {
             t = profiling_now();
             LL_ATON_OSAL_WFE();
             profiling_wfe(PROF_NET_FC, profiling_now() - t);
         }
     } while (ret != LL_ATON_RT_DONE);

     t = profiling_now();
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
     profiling_npu_post(PROF_NET_FC, profiling_now() - t);

     t = profiling_now();
     LL_ATON_RT_DeInit_Network(&NN_Instance_network_fc);
     profiling_npu_deinit(PROF_NET_FC, profiling_now() - t);

     profiling_npu_wall(PROF_NET_FC, profiling_tick_now() - wall0);
 }

 void MX_X_CUBE_AI_Process_User(ai_result_t *result)
 {

     LL_ATON_RT_RetValues_t ll_aton_rt_ret;
     uint32_t wall0 = profiling_tick_now();
     uint64_t t;

     t = profiling_now();
     LL_ATON_RT_Init_Network(&NN_Instance_network_f);
     profiling_npu_init(PROF_NET_F, profiling_now() - t);

     do {
         t = profiling_now();
         ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_network_f);
         profiling_npu_runblock(PROF_NET_F, profiling_now() - t);
         if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
             t = profiling_now();
             LL_ATON_OSAL_WFE();
             profiling_wfe(PROF_NET_F, profiling_now() - t);
         }
     } while (ll_aton_rt_ret != LL_ATON_RT_DONE);

     t = profiling_now();
     ai_blazeface_postprocess(result);
     profiling_npu_post(PROF_NET_F, profiling_now() - t);

     t = profiling_now();
     LL_ATON_RT_DeInit_Network(&NN_Instance_network_f);
     profiling_npu_deinit(PROF_NET_F, profiling_now() - t);

     profiling_npu_wall(PROF_NET_F, profiling_tick_now() - wall0);
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

    /* Register epoch callbacks after the discovery Init/DeInit above so the
       boot-time probe does not pollute the counters. */
    profiling_reset();
    LL_ATON_RT_SetNetworkCallback(&NN_Instance_network_f,  prof_epoch_cb);
    LL_ATON_RT_SetNetworkCallback(&NN_Instance_network_fc, prof_epoch_cb);

    printf("[FC-INIT] buffer_in_fc=0x%08lX buffer_out_fc=0x%08lX\r\n",
           (unsigned long)buffer_in_fc, (unsigned long)buffer_out_fc);
    printf("[FC-INIT] NOR Flash weight @ 0x71200000 + 1156289 bytes\r\n");

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
