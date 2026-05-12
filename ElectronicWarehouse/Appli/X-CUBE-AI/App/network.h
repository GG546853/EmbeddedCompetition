/**
  ******************************************************************************
  * @file    network.h
  * @author  STEdgeAI
  * @date    2026-05-12 15:47:59
  * @brief   Minimal description of the generated c-implemention of the network
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */
#ifndef LL_ATON_DEFAULT_H
#define LL_ATON_DEFAULT_H

/******************************************************************************/
#define LL_ATON_DEFAULT_C_MODEL_NAME        "Default"
#define LL_ATON_DEFAULT_ORIGIN_MODEL_NAME   "yunetn_320_qdq_int8"

/************************** USER ALLOCATED IOs ********************************/
// No user allocated inputs
// No user allocated outputs

/************************** INPUTS ********************************************/
#define LL_ATON_DEFAULT_IN_NUM        (1)    // Total number of input buffers
// Input buffer 1 -- Input_0_out_0
#define LL_ATON_DEFAULT_IN_1_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_IN_1_SIZE_BYTES  (307200)

/************************** OUTPUTS *******************************************/
#define LL_ATON_DEFAULT_OUT_NUM        (12)    // Total number of output buffers
// Output buffer 1 -- Quantize_133_out_0
#define LL_ATON_DEFAULT_OUT_1_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_1_SIZE_BYTES  (1600)
// Output buffer 2 -- Quantize_180_out_0
#define LL_ATON_DEFAULT_OUT_2_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_2_SIZE_BYTES  (400)
// Output buffer 3 -- Quantize_227_out_0
#define LL_ATON_DEFAULT_OUT_3_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_3_SIZE_BYTES  (100)
// Output buffer 4 -- Quantize_143_out_0
#define LL_ATON_DEFAULT_OUT_4_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_4_SIZE_BYTES  (1600)
// Output buffer 5 -- Quantize_190_out_0
#define LL_ATON_DEFAULT_OUT_5_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_5_SIZE_BYTES  (400)
// Output buffer 6 -- Quantize_237_out_0
#define LL_ATON_DEFAULT_OUT_6_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_6_SIZE_BYTES  (100)
// Output buffer 7 -- Quantize_153_out_0
#define LL_ATON_DEFAULT_OUT_7_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_7_SIZE_BYTES  (6400)
// Output buffer 8 -- Quantize_200_out_0
#define LL_ATON_DEFAULT_OUT_8_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_8_SIZE_BYTES  (1600)
// Output buffer 9 -- Quantize_247_out_0
#define LL_ATON_DEFAULT_OUT_9_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_9_SIZE_BYTES  (400)
// Output buffer 10 -- Quantize_163_out_0
#define LL_ATON_DEFAULT_OUT_10_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_10_SIZE_BYTES  (16000)
// Output buffer 11 -- Quantize_210_out_0
#define LL_ATON_DEFAULT_OUT_11_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_11_SIZE_BYTES  (4000)
// Output buffer 12 -- Quantize_257_out_0
#define LL_ATON_DEFAULT_OUT_12_ALIGNMENT   (32)
#define LL_ATON_DEFAULT_OUT_12_SIZE_BYTES  (1000)

#endif /* LL_ATON_DEFAULT_H */
