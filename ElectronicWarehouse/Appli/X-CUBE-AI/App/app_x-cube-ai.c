
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
#include"dma2d.h"
#include <math.h>

 typedef struct {
     float x1, y1, x2, y2;   // 矩形框
     float score;            // 最终得分 (cls * obj)
     struct {
         float x, y;
     } landmarks[5];         // 5个关键点
     int keep;               // NMS 标志位
 } FaceDetection;

 typedef struct {
     int stride;
     int map_size;
     // 6个Tensor的索引 (基于12个输出的典型排列)
     int cls_idx; // 分类
     int obj_idx; // 目标存在感
     int reg_idx; // 框回归
     int kps_idx; // 关键点回归
 } YuNetLayer;

 float scales[12] = {
     0.003292752, 0.003212299, 0.002707064, // 0,1,2: Cls
     0.003796757, 0.003871595, 0.000669989, // 3,4,5: Obj  <-- 已修复
     0.011400674, 0.011022569, 0.012532321, // 6,7,8: Reg
     0.019418273, 0.015351823, 0.016416648  // 9,10,11:Kps <-- 已修复
 };
 int32_t zps[12] = {-128, -128, -128, -128, -128, -128, -61, -46, -16, -26, -33, -45};

#define CONF_THRESH 0.5f
#define MAX_CANDIDATES 1024


 //extern uint8_t g_ai_cam_buf[];
 extern uint8_t g_ltdc_layer2_framebuf[480 * 800 * 3];
 extern DCMIPP_HandleTypeDef hdcmipp;
 extern osSemaphoreId_t cam_frame_sem; // 确保在 main.c 中创建了这个信号量并在此声明
 extern DMA2D_HandleTypeDef hdma2d;
 extern uint8_t g_ai_cam_buf[320 * 320 * 3];

  __attribute__((section(".camera_buf")))
 __attribute__((aligned(32)))

 FaceDetection boxes[200];

#define CONF_THRESHOLD 0.5f  // 置信度阈值
#define MAX_RESULTS 200      // 结果数组最大长度
#define INPUT_SIZE 320


  int yunet_decode(const LL_Buffer_InfoTypeDef* obuffersInfos, FaceDetection* results) {
      int valid_count = 0;

      // 定义三层 Stride 的结构体
      YuNetLayer layers[3] = {
          {8,  40, 0, 3, 6, 9},   // Stride 8:  1600个anchor
          {16, 20, 1, 4, 7, 10},  // Stride 16: 400个anchor
          {32, 10, 2, 5, 8, 11}   // Stride 32: 100个anchor
      };

      for (int l = 0; l < 3; l++) {
          YuNetLayer layer = layers[l];
          int stride = layer.stride;
          int map_size = layer.map_size;

          // 获取对应的 int8 原始数据指针
          int8_t* cls_ptr = (int8_t*)LL_Buffer_addr_start(&obuffersInfos[layer.cls_idx]);
          int8_t* obj_ptr = (int8_t*)LL_Buffer_addr_start(&obuffersInfos[layer.obj_idx]);
          int8_t* reg_ptr = (int8_t*)LL_Buffer_addr_start(&obuffersInfos[layer.reg_idx]);
          int8_t* kps_ptr = (int8_t*)LL_Buffer_addr_start(&obuffersInfos[layer.kps_idx]);

          // 遍历特征图 (row x col)
          for (int y = 0; y < map_size; y++) {
              for (int x = 0; x < map_size; x++) {
                  int anchor_idx = y * map_size + x;

                  // 1. 反量化并计算最终得分 (Score = Cls * Obj)
                  float cls_score = (cls_ptr[anchor_idx] - zps[layer.cls_idx]) * scales[layer.cls_idx];
                  float obj_score = (obj_ptr[anchor_idx] - zps[layer.obj_idx]) * scales[layer.obj_idx];
                  float final_score = cls_score * obj_score;

                  // 阈值过滤
                  if (final_score < CONF_THRESHOLD) continue;
                  // 过滤饱和锚点: cls或obj达到int8上限(127)说明量化溢出，结果不可靠
                  if (cls_ptr[anchor_idx] == 127 || obj_ptr[anchor_idx] == 127) continue;
                  if (valid_count >= MAX_RESULTS) return valid_count;

                  // 2. 解码边界框 (Bounding Box)
                  // reg_ptr 包含 4 个值: [dx, dy, dw, dh]
                  float dx = (reg_ptr[anchor_idx * 4 + 0] - zps[layer.reg_idx]) * scales[layer.reg_idx];
                  float dy = (reg_ptr[anchor_idx * 4 + 1] - zps[layer.reg_idx]) * scales[layer.reg_idx];
                  float dw = (reg_ptr[anchor_idx * 4 + 2] - zps[layer.reg_idx]) * scales[layer.reg_idx];
                  float dh = (reg_ptr[anchor_idx * 4 + 3] - zps[layer.reg_idx]) * scales[layer.reg_idx];

                  // 计算中心点坐标 (像素)
                  float cx = (dx * stride) + (x * stride);
                  float cy = (dy * stride) + (y * stride);
                  // 计算宽高 (使用 exp 还原)
                  float w = expf(dw) * stride;
                  float h = expf(dh) * stride;

                  // 转为矩形框 (x1, y1, x2, y2)
                  results[valid_count].x1 = cx - w / 2.0f;
                  results[valid_count].y1 = cy - h / 2.0f;
                  results[valid_count].x2 = cx + w / 2.0f;
                  results[valid_count].y2 = cy + h / 2.0f;
                  results[valid_count].score = final_score;

                  if (valid_count < 5) {
                      printf("DEBUG anchor[%d]: cls_raw=%d, obj_raw=%d, dx=%.4f(reg=%d), dw=%.4f(reg=%d), score=%.4f\r\n",
                          anchor_idx,
                          cls_ptr[anchor_idx], obj_ptr[anchor_idx],
                          dx, reg_ptr[anchor_idx * 4 + 0],
                          dw, reg_ptr[anchor_idx * 4 + 2],
                          final_score);
                  }
                  results[valid_count].keep = 1;

                  // 3. 解码 5 个关键点 (Landmarks)
                  // kps_ptr 包含 10 个值: [dx0, dy0, dx1, dy1, ...]
                  for (int k = 0; k < 5; k++) {
                      float kpx = (kps_ptr[anchor_idx * 10 + k * 2 + 0] - zps[layer.kps_idx]) * scales[layer.kps_idx];
                      float kpy = (kps_ptr[anchor_idx * 10 + k * 2 + 1] - zps[layer.kps_idx]) * scales[layer.kps_idx];

                      results[valid_count].landmarks[k].x = (kpx * stride) + (x * stride);
                      results[valid_count].landmarks[k].y = (kpy * stride) + (y * stride);
                  }

                  valid_count++;
              }
          }
      }

      return valid_count;
  }
  void do_nms(FaceDetection* boxes, int count, float iou_thresh) {
      // 1. 冒泡排序（按 score 从高到低）
      for (int i = 0; i < count - 1; i++) {
          for (int j = 0; j < count - i - 1; j++) {
              if (boxes[j].score < boxes[j+1].score) {
                  FaceDetection temp = boxes[j];
                  boxes[j] = boxes[j+1];
                  boxes[j+1] = temp;
              }
          }
      }

      // 2. IoU 计算与剔除
      for (int i = 0; i < count; i++) {
          if (!boxes[i].keep) continue;
          for (int j = i + 1; j < count; j++) {
              if (!boxes[j].keep) continue;

              float xx1 = fmaxf(boxes[i].x1, boxes[j].x1);
              float yy1 = fmaxf(boxes[i].y1, boxes[j].y1);
              float xx2 = fminf(boxes[i].x2, boxes[j].x2);
              float yy2 = fminf(boxes[i].y2, boxes[j].y2);

              float w = fmaxf(0, xx2 - xx1);
              float h = fmaxf(0, yy2 - yy1);
              float inter = w * h;
              if (inter > 0) {
                  float area_i = (boxes[i].x2 - boxes[i].x1) * (boxes[i].y2 - boxes[i].y1);
                  float area_j = (boxes[j].x2 - boxes[j].x1) * (boxes[j].y2 - boxes[j].y1);
                  float iou = inter / (area_i + area_j - inter);
                  if (iou > iou_thresh) boxes[j].keep = 0;
              }
          }
      }
  }


  // 伪代码：将 NHWC (320*320*3) 转换为 NCHW (3*320*320)
  // 同时完成 uint8 到 int8 的转换 (假设 offset 是 -128)
  void transpose_rgb_to_nchw(uint8_t* src, int8_t* dst, int width, int height) {
      int image_size = width * height;
      // Model expects BGR channel order (OpenCV convention), camera DCMIPP outputs RGB.
      // NCHW layout: plane 0 = B, plane 1 = G, plane 2 = R
      int8_t* dst_b = dst;                  // model channel 0 (B)
      int8_t* dst_g = dst + image_size;     // model channel 1 (G)
      int8_t* dst_r = dst + 2 * image_size; // model channel 2 (R)

      for (int i = 0; i < image_size; i++) {
          // Camera RGB → Model BGR: swap R and B
          dst_b[i] = (int8_t)((int)src[i * 3 + 2] - 128); // camera B → model B plane
          dst_g[i] = (int8_t)((int)src[i * 3 + 1] - 128); // camera G → model G plane
          dst_r[i] = (int8_t)((int)src[i * 3 + 0] - 128); // camera R → model R plane
      }
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
    __HAL_RCC_XSPI1_CLK_SLEEP_ENABLE();    /* For display frame buffer */
    __HAL_RCC_XSPI2_CLK_SLEEP_ENABLE();    /* For NN weights */
    __HAL_RCC_NPU_CLK_SLEEP_ENABLE();      /* For NN inference */
    __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE(); /* For NN inference */
    __HAL_RCC_LTDC_CLK_SLEEP_ENABLE();     /* For display */
    __HAL_RCC_DMA2D_CLK_SLEEP_ENABLE();    /* For display */
    __HAL_RCC_DCMIPP_CLK_SLEEP_ENABLE();   /* For camera configuration retention */
    __HAL_RCC_CSI_CLK_SLEEP_ENABLE();      /* For camera configuration retention */

    LL_ATON_RT_RuntimeInit();
    LL_ATON_RT_Init_Network(&NN_Instance_Default);
    /* USER CODE END 5 */
}

void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 6 */
    uint32_t buff_in_len;
    LL_ATON_RT_RetValues_t ll_aton_rt_ret = LL_ATON_RT_DONE;
    const LL_Buffer_InfoTypeDef * ibuffersInfos = NN_Interface_Default.input_buffers_info();
    const LL_Buffer_InfoTypeDef * obuffersInfos = NN_Interface_Default.output_buffers_info();
    buffer_in = (uint8_t *)LL_Buffer_addr_start(&ibuffersInfos[0]);

    LL_ATON_RT_RuntimeInit();
    buff_in_len = ibuffersInfos->offset_end - ibuffersInfos->offset_start;

    // 清理输入 Cache
    SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
    SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);

    // ===== 第3点 T1: 发起快照前 =====
    TickType_t t1 = xTaskGetTickCount();

    if (HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0, (uint32_t)g_ai_cam_buf, DCMIPP_MODE_SNAPSHOT) != HAL_OK) {
        printf("ERROR: DCMIPP PIPE2 Start Failed!\r\n");
    }

    if(osSemaphoreAcquire(cam_frame_sem, pdMS_TO_TICKS(100)) != osOK)
    	return;

    // ===== 第3点 T2: 拿到信号量 =====
    TickType_t t2 = xTaskGetTickCount();

    SCB_InvalidateDCache_by_Addr((uint32_t*)g_ai_cam_buf, 320 * 320 * 3);
    transpose_rgb_to_nchw(g_ai_cam_buf, buffer_in, 320, 320);

    // ===== 第3点 T3: 预处理完成 =====
    TickType_t t3 = xTaskGetTickCount();

    SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);


    for (int inferenceNb = 0; inferenceNb < 1; ++inferenceNb) {
        SCB_CleanDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);
        SCB_InvalidateDCache_by_Addr((uint32_t*)buffer_in, buff_in_len);

        do {
            ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);
            if (ll_aton_rt_ret == LL_ATON_RT_WFE) {
                LL_ATON_OSAL_WFE();
            }
        } while (ll_aton_rt_ret != LL_ATON_RT_DONE);

        for (int i = 0; i < 12; i++) {
            uint32_t addr = (uint32_t)LL_Buffer_addr_start(&obuffersInfos[i]);
            uint32_t len = obuffersInfos[i].offset_end - obuffersInfos[i].offset_start;
            SCB_InvalidateDCache_by_Addr((uint32_t*)addr, len);
        }

        // ===== 第3点 T4: NPU推理完成 =====
        TickType_t t4 = xTaskGetTickCount();
        printf("Timing: T1(snap->)=%lu T2(snapOK)=%lu T3(prepOK)=%lu T4(npudone)=%lu  |  snap=%lums prep=%lums infer=%lums\r\n",
            (unsigned long)t1, (unsigned long)t2, (unsigned long)t3, (unsigned long)t4,
            (unsigned long)(t2 - t1), (unsigned long)(t3 - t2), (unsigned long)(t4 - t3));

        printf("\r\n========== Frame Stats ==========\r\n");

        // ===== 第1点 & 第2点: 三个stride的cls/obj统计 + 饱和计数 =====
        int strides[3] = {8, 16, 32};
        int map_sizes[3] = {40, 20, 10};
        int cls_indices[3] = {0, 1, 2};
        int obj_indices[3] = {3, 4, 5};

        for (int s = 0; s < 3; s++) {
            int8_t* cls_ptr = (int8_t*)LL_Buffer_addr_start(&obuffersInfos[cls_indices[s]]);
            int8_t* obj_ptr = (int8_t*)LL_Buffer_addr_start(&obuffersInfos[obj_indices[s]]);
            int total = map_sizes[s] * map_sizes[s];

            int cls_min = 127, cls_max = -128, cls_sat = 0;
            int obj_min = 127, obj_max = -128, obj_sat = 0;
            long cls_sum = 0, obj_sum = 0;

            for (int i = 0; i < total; i++) {
                if (cls_ptr[i] < cls_min) cls_min = cls_ptr[i];
                if (cls_ptr[i] > cls_max) cls_max = cls_ptr[i];
                if (cls_ptr[i] == 127) cls_sat++;
                cls_sum += cls_ptr[i];

                if (obj_ptr[i] < obj_min) obj_min = obj_ptr[i];
                if (obj_ptr[i] > obj_max) obj_max = obj_ptr[i];
                if (obj_ptr[i] == 127) obj_sat++;
                obj_sum += obj_ptr[i];
            }

            printf("Stride %2d (idx %d/%d): cls min=%d max=%d avg=%ld sat127=%d  |  obj min=%d max=%d avg=%ld sat127=%d\r\n",
                strides[s], cls_indices[s], obj_indices[s],
                cls_min, cls_max, cls_sum / total, cls_sat,
                obj_min, obj_max, obj_sum / total, obj_sat);
        }

        // stride-8 前20个 raw 值保留
        {
            int8_t* cls8 = (int8_t*)LL_Buffer_addr_start(&obuffersInfos[0]);
            int8_t* obj8 = (int8_t*)LL_Buffer_addr_start(&obuffersInfos[3]);
            printf("cls[0..19] S8: ");
            for (int i = 0; i < 20; i++) printf("%d ", cls8[i]);
            printf("\r\nobj[0..19] S8: ");
            for (int i = 0; i < 20; i++) printf("%d ", obj8[i]);
            printf("\r\n");
        }

        printf("==================================\r\n\r\n");



        int count = yunet_decode(obuffersInfos, boxes);
        printf("Valid detection candidates: %d\n\r", count);
        do_nms(boxes, count, 0.45f);

        printf("--- boxes[0-5]---\r\n");
        for (int i = 0; i < 6; i++) {
            printf("boxes[%d]: score:%.4f, x1=%.4f, y1=%.4f, x2=%.4f, y2=%.4f\r\n", i, boxes[i].score,boxes[i].x1,boxes[i].y1,boxes[i].x2,boxes[i].y2);
        }

        printf("------------------------------\r\n");

//        printf("--- AI Model Outputs Check ---\r\n");
//        for (int i = 0; i < 12; i++) {
//            uint32_t size = obuffersInfos[i].offset_end - obuffersInfos[i].offset_start;
//            printf("Index [%d]: Size = %lu bytes\r\n", i, size);
//        }
//        printf("------------------------------\r\n");


        int final_count = 0;

        hdma2d.Init.Mode = DMA2D_R2M;                 // 寄存器到内存(纯色填充)
        hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;  // 设定输出颜色格式
        hdma2d.Init.OutputOffset = 0;                 // 【关键】重置偏移量为0，否则清屏会错乱
        if (HAL_DMA2D_Init(&hdma2d) != HAL_OK) {
            // 初始化错误处理
        }
        HAL_DMA2D_Start(&hdma2d, 0x00000000, (uint32_t)g_ltdc_layer2_framebuf, 800, 480);
        HAL_DMA2D_PollForTransfer(&hdma2d, 100);

        for (int i = 0; i < count; i++) {
        	if(boxes[i].keep){
        		final_count++;
        		int display_x1 = (int)(boxes[i].x1 * 2.5);
        		int display_y1 = (int)(boxes[i].y1 * 1.5);
        		int display_x2 = (int)(boxes[i].x2 * 2.5);
        		int display_y2 = (int)(boxes[i].y2 * 1.5);
        		int display_width = display_x2 - display_x1;
        		int display_height = display_y2 - display_y1;

        		if (display_x1 < 0) display_x1 = 0;
        		if (display_y1 < 0) display_y1 = 0;
        		if (display_x2 > 800) display_x2 = 800;
        		if (display_y2 > 480) display_y2 = 480;
        		display_width = display_x2 - display_x1;
        		display_height = display_y2 - display_y1;
        		if (display_width < 0) display_width = 0;
        		if (display_height < 0) display_height = 0;
        		if (display_width > 0 && display_height > 0) {
        			if (display_y1 >= 480) display_y1 = 479;
        			if (display_y2 > 480) display_y2 = 480;
        			if (display_x1 >= 800) display_x1 = 799;
        			if (display_x2 > 800) display_x2 = 800;
        			display_width = display_x2 - display_x1;
        			display_height = display_y2 - display_y1;
        			if (display_width <= 0 || display_height <= 0) continue;

        			if (display_y1 < 480) {
        				hdma2d.Init.Mode = DMA2D_R2M;
        				hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
        				hdma2d.Init.OutputOffset = 800 - display_width;
        				hdma2d.Init.RedBlueSwap = DMA2D_RB_REGULAR;
        				HAL_DMA2D_Init(&hdma2d);
        				HAL_DMA2D_ConfigLayer(&hdma2d, 1);
        				HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[(display_y1 * 800 + display_x1) * 3], display_width, 1);
        				HAL_DMA2D_PollForTransfer(&hdma2d, 100);
        			}
        			if (display_y2  <= 480 && display_y2 > display_y1) {
        				hdma2d.Init.OutputOffset = 800 - display_width;
        				HAL_DMA2D_Init(&hdma2d);
        				HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[((display_y2 - 1) * 800 + display_x1) * 3], display_width, 1);
        				HAL_DMA2D_PollForTransfer(&hdma2d, 100);
        			}
        			if (display_x1  < 800) {
        				hdma2d.Init.OutputOffset = 800 - 1;
        				HAL_DMA2D_Init(&hdma2d);
        				HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[(display_y1 * 800 + display_x1) * 3], 1, display_height);
        				HAL_DMA2D_PollForTransfer(&hdma2d, 100);
        			}
        			if (display_x2 <= 800 && display_x2 > display_x1) {
        				hdma2d.Init.OutputOffset = 800 - 1;
        				HAL_DMA2D_Init(&hdma2d);
        				HAL_DMA2D_Start(&hdma2d, 0x00FF0000, (uint32_t)&g_ltdc_layer2_framebuf[(display_y1 * 800 + (display_x2 - 1)) * 3], 1, display_height);
        				HAL_DMA2D_PollForTransfer(&hdma2d, 100);
        			}
        		}

        	}
        }


            LL_ATON_RT_Reset_Network(&NN_Instance_Default);
            vTaskDelay(pdMS_TO_TICKS(10));
    }
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
