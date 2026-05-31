/**
  ******************************************************************************
  * @file    network.h
  * @author  STEdgeAI
  * @date    2026-05-31 10:26:02
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
#ifndef LL_ATON_NETWORK_H
#define LL_ATON_NETWORK_H

/******************************************************************************/
#define LL_ATON_NETWORK_C_MODEL_NAME        "network"
#define LL_ATON_NETWORK_ORIGIN_MODEL_NAME   "blazeface_front_128_quant_pc_ff_od_wider_face"

/************************** USER ALLOCATED IOs ********************************/
#define LL_ATON_NETWORK_USER_ALLOCATED_INPUTS   (1)  // Number of input buffers not allocated by the compiler
#define LL_ATON_NETWORK_USER_ALLOCATED_OUTPUTS  (4)  // Number of output buffers not allocated by the compiler

/************************** INPUTS ********************************************/
#define LL_ATON_NETWORK_IN_NUM        (1)    // Total number of input buffers
// Input buffer 1 -- Input_0_out_0
#define LL_ATON_NETWORK_IN_1_ALIGNMENT   (32)
#define LL_ATON_NETWORK_IN_1_SIZE_BYTES  (49152)

/************************** OUTPUTS *******************************************/
#define LL_ATON_NETWORK_OUT_NUM        (4)    // Total number of output buffers
// Output buffer 1 -- Transpose_246_out_0
#define LL_ATON_NETWORK_OUT_1_ALIGNMENT   (32)
#define LL_ATON_NETWORK_OUT_1_SIZE_BYTES  (8192)
// Output buffer 2 -- Transpose_254_out_0
#define LL_ATON_NETWORK_OUT_2_ALIGNMENT   (32)
#define LL_ATON_NETWORK_OUT_2_SIZE_BYTES  (512)
// Output buffer 3 -- Transpose_238_out_0
#define LL_ATON_NETWORK_OUT_3_ALIGNMENT   (32)
#define LL_ATON_NETWORK_OUT_3_SIZE_BYTES  (384)
// Output buffer 4 -- Transpose_230_out_0
#define LL_ATON_NETWORK_OUT_4_ALIGNMENT   (32)
#define LL_ATON_NETWORK_OUT_4_SIZE_BYTES  (6144)

#endif /* LL_ATON_NETWORK_H */
