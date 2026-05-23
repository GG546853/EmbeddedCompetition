#include "main.h"
#include "fd_blazeface_pp_f32.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
/*  Helper: sigmoid                                                          */
/* -------------------------------------------------------------------------- */
static inline float32_t sigmoid_f(float32_t x)
{
  return (1.0f / (1.0f + expf(-x)));
}

/* -------------------------------------------------------------------------- */
/*  Helper: box IoU (boxes as [x_center, y_center, width, height])           */
/* -------------------------------------------------------------------------- */
static float32_t box_iou(float32_t *a, float32_t *b)
{
  float32_t a_x1 = a[0] - a[2] * 0.5f;
  float32_t a_y1 = a[1] - a[3] * 0.5f;
  float32_t a_x2 = a[0] + a[2] * 0.5f;
  float32_t a_y2 = a[1] + a[3] * 0.5f;

  float32_t b_x1 = b[0] - b[2] * 0.5f;
  float32_t b_y1 = b[1] - b[3] * 0.5f;
  float32_t b_x2 = b[0] + b[2] * 0.5f;
  float32_t b_y2 = b[1] + b[3] * 0.5f;

  float32_t inter_x1 = (a_x1 > b_x1) ? a_x1 : b_x1;
  float32_t inter_y1 = (a_y1 > b_y1) ? a_y1 : b_y1;
  float32_t inter_x2 = (a_x2 < b_x2) ? a_x2 : b_x2;
  float32_t inter_y2 = (a_y2 < b_y2) ? a_y2 : b_y2;

  float32_t inter_w = inter_x2 - inter_x1;
  float32_t inter_h = inter_y2 - inter_y1;
  if (inter_w <= 0.0f || inter_h <= 0.0f) return 0.0f;

  float32_t inter_area = inter_w * inter_h;
  float32_t a_area = a[2] * a[3];
  float32_t b_area = b[2] * b[3];
  float32_t union_area = a_area + b_area - inter_area;
  if (union_area <= 0.0f) return 0.0f;

  return inter_area / union_area;
}

/* -------------------------------------------------------------------------- */
/*  NMS comparator for qsort                                                  */
/* -------------------------------------------------------------------------- */
static int32_t nms_comparator(const void *pa, const void *pb)
{
  fd_pp_outBuffer_t *a = (fd_pp_outBuffer_t *)pa;
  fd_pp_outBuffer_t *b = (fd_pp_outBuffer_t *)pb;
  float32_t diff = a->conf - b->conf;
  if (diff < 0) return 1;
  if (diff > 0) return -1;
  return 0;
}

/* -------------------------------------------------------------------------- */
/*  NMS filtering                                                             */
/* -------------------------------------------------------------------------- */
static void nms_filter(fd_pp_out_t *pOutput, int32_t nb_detect,
                       float32_t iou_threshold, int32_t max_boxes_limit)
{
  if (nb_detect <= 0) return;

  /* Sort by confidence descending */
  qsort(pOutput->pOutBuff, nb_detect, sizeof(fd_pp_outBuffer_t), nms_comparator);

  /* Suppress overlapping boxes */
  for (int32_t i = 0; i < nb_detect; i++) {
    if (pOutput->pOutBuff[i].conf == 0.0f) continue;
    float32_t *a = &(pOutput->pOutBuff[i].x_center);
    for (int32_t j = i + 1; j < nb_detect; j++) {
      if (pOutput->pOutBuff[j].conf == 0.0f) continue;
      float32_t *b = &(pOutput->pOutBuff[j].x_center);
      if (box_iou(a, b) > iou_threshold) {
        pOutput->pOutBuff[j].conf = 0.0f;
      }
    }
  }

  /* Limit to max_boxes_limit */
  int32_t kept = 0;
  for (int32_t i = 0; i < nb_detect; i++) {
    if (pOutput->pOutBuff[i].conf > 0.0f) {
      if (kept >= max_boxes_limit) {
        pOutput->pOutBuff[i].conf = 0.0f;
      }
      kept++;
    }
  }
}

/* -------------------------------------------------------------------------- */
/*  Decode one detection level (float32 outputs — already dequantized)        */
/* -------------------------------------------------------------------------- */
static int32_t decode_level_f32(float32_t *pRawBoxes, float32_t *pScores,
                                fd_pp_out_t *pOutput,
                                const float32_t *pAnchors,
                                int32_t nb_detections,
                                int32_t nb_keypoints,
                                int32_t in_size,
                                float32_t conf_threshold,
                                int32_t start_idx)
{
  float32_t inv_size = 1.0f / (float32_t)in_size;
  int32_t box_stride = nb_keypoints * 2 + FD_BLAZEFACE_KEYPOINTS; /* 16 */
  int32_t det_count = start_idx;

  /* Pre-compute logit threshold: sigmoid(x) > conf_threshold  =>  x > ln(conf_threshold/(1-conf_threshold)) */
  float32_t logit_thresh = logf(conf_threshold / (1.0f - conf_threshold));

  fd_pp_outBuffer_t *pOutBuff = pOutput->pOutBuff;

  for (int32_t i = 0; i < nb_detections; i++) {
    float32_t raw_score = pScores[i];

    /* Early reject with logit threshold */
    if (raw_score <= logit_thresh) {
      pRawBoxes += box_stride;
      pAnchors += 2;
      continue;
    }

    float32_t prob = sigmoid_f(raw_score);

    pOutBuff[det_count].conf = prob;
    pOutBuff[det_count].class_index = 0;

    /* Decode bounding box */
    pOutBuff[det_count].x_center = pRawBoxes[FD_BLAZEFACE_XCENTER]   * inv_size + pAnchors[0];
    pOutBuff[det_count].y_center = pRawBoxes[FD_BLAZEFACE_YCENTER]   * inv_size + pAnchors[1];
    pOutBuff[det_count].width    = pRawBoxes[FD_BLAZEFACE_WIDTH]     * inv_size;
    pOutBuff[det_count].height   = pRawBoxes[FD_BLAZEFACE_HEIGHT]    * inv_size;

    /* Decode keypoints */
    for (int32_t k = 0; k < nb_keypoints; k++) {
      pOutBuff[det_count].pKeyPoints[k].x = pRawBoxes[FD_BLAZEFACE_KEYPOINTS + 2*k]     * inv_size + pAnchors[0];
      pOutBuff[det_count].pKeyPoints[k].y = pRawBoxes[FD_BLAZEFACE_KEYPOINTS + 2*k + 1] * inv_size + pAnchors[1];
    }

    det_count++;
    pRawBoxes += box_stride;
    pAnchors += 2;
  }

  return det_count;
}

/* -------------------------------------------------------------------------- */
/*  Main post-processing entry point                                          */
/* -------------------------------------------------------------------------- */
int32_t fd_blazeface_pp_process_f32(fd_blazeface_pp_f32_in_t *pInput,
                                    fd_pp_out_t *pOutput,
                                    fd_blazeface_pp_f32_static_param_t *pParams)
{
  if (pOutput->pOutBuff == NULL) return AI_FD_PP_ERROR;

  int32_t nb_detect = 0;

  /* Decode level 0 (stride 8): 512 anchors */
  nb_detect = decode_level_f32(pInput->pRawBoxes_0,
                               pInput->pScores_0,
                               pOutput,
                               pParams->pAnchors_0,
                               pParams->nb_detections_0,
                               pParams->nb_keypoints,
                               pParams->in_size,
                               pParams->conf_threshold,
                               nb_detect);

  /* Decode level 1 (stride 16): 384 anchors, appended after level 0 */
  nb_detect = decode_level_f32(pInput->pRawBoxes_1,
                               pInput->pScores_1,
                               pOutput,
                               pParams->pAnchors_1,
                               pParams->nb_detections_1,
                               pParams->nb_keypoints,
                               pParams->in_size,
                               pParams->conf_threshold,
                               nb_detect);

  /* NMS */
  nms_filter(pOutput, nb_detect, pParams->iou_threshold, pParams->max_boxes_limit);

  /* Compact valid detections after NMS */
  int32_t valid_count = 0;
  for (int32_t i = 0; i < nb_detect; i++) {
    if (pOutput->pOutBuff[i].conf > 0.0f) {
      if (valid_count != i) {
        memcpy(&pOutput->pOutBuff[valid_count], &pOutput->pOutBuff[i], sizeof(fd_pp_outBuffer_t));
      }
      valid_count++;
    }
  }
  pOutput->nb_detect = valid_count;

  return AI_FD_PP_ERROR_NO;
}
