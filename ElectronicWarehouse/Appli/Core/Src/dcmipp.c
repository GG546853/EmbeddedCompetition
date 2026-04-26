/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dcmipp.c
  * @brief   This file provides code for the configuration
  *          of the DCMIPP instances.
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "dcmipp.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

DCMIPP_HandleTypeDef hdcmipp;

/* DCMIPP init function */
void MX_DCMIPP_Init(void)
{

  /* USER CODE BEGIN DCMIPP_Init 0 */

  /* USER CODE END DCMIPP_Init 0 */

  DCMIPP_CSI_PIPE_ConfTypeDef pCSI_PipeConfig = {0};
  DCMIPP_CSI_ConfTypeDef pCSI_Config = {0};
  DCMIPP_PipeConfTypeDef pPipeConfig = {0};

  /* USER CODE BEGIN DCMIPP_Init 1 */

  /* USER CODE END DCMIPP_Init 1 */
  hdcmipp.Instance = DCMIPP;
  if (HAL_DCMIPP_Init(&hdcmipp) != HAL_OK)
  {
    Error_Handler();
  }

  /** Pipe 1 Config
  */
  pCSI_PipeConfig.DataTypeMode = DCMIPP_DTMODE_DTIDA;
  pCSI_PipeConfig.DataTypeIDA = DCMIPP_DT_YUV420_8;
  pCSI_PipeConfig.DataTypeIDB = DCMIPP_DT_YUV420_8;
  if (HAL_DCMIPP_CSI_PIPE_SetConfig(&hdcmipp, DCMIPP_PIPE1, &pCSI_PipeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  pCSI_Config.PHYBitrate = DCMIPP_CSI_PHY_BT_80;
  pCSI_Config.DataLaneMapping = DCMIPP_CSI_PHYSICAL_DATA_LANES;
  pCSI_Config.NumberOfLanes = DCMIPP_CSI_ONE_DATA_LANE;
  HAL_DCMIPP_CSI_SetConfig(&hdcmipp, &pCSI_Config);
  pPipeConfig.FrameRate = DCMIPP_FRAME_RATE_ALL;
  pPipeConfig.PixelPipePitch = 10;
  pPipeConfig.PixelPackerFormat = DCMIPP_PIXEL_PACKER_FORMAT_RGB888_YUV444_1;
  if (HAL_DCMIPP_PIPE_SetConfig(&hdcmipp, DCMIPP_PIPE1, &pPipeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DCMIPP_CSI_SetVCConfig(&hdcmipp, 0U, DCMIPP_CSI_DT_BPP6) != HAL_OK)
  {
    Error_Handler();
  }

  /** Pipe 2 Config
  */
  pCSI_PipeConfig.DataTypeIDB = DCMIPP_DT_RGB565;
  if (HAL_DCMIPP_CSI_PIPE_SetConfig(&hdcmipp, DCMIPP_PIPE2, &pCSI_PipeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DCMIPP_PIPE_SetConfig(&hdcmipp, DCMIPP_PIPE2, &pPipeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DCMIPP_Init 2 */

  /* USER CODE END DCMIPP_Init 2 */

}

/* USER CODE BEGIN 1 */
/**
* @brief DCMIPP MSP Initialization
* 针对正点原子 N647 开发板 + IMX335 摄像头模块
* 适配 FreeRTOS 环境，确保时钟频率与 MIPI 物理层匹配
*/
void HAL_DCMIPP_MspInit(DCMIPP_HandleTypeDef *hdcmipp)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  if(hdcmipp->Instance == DCMIPP)
  {
    /* 1. 配置外设时钟树 (非常关键，决定了 MIPI 能否同步) */
    /* 注意：这里的时钟源和分频系数需确保能支持 IMX335 的 1600Mbps 速率 */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_DCMIPP | RCC_PERIPHCLK_CSI;

    // DCMIPP 使用 IC17 时钟通道，来源于 PLL1，4分频
    PeriphClkInitStruct.DcmippClockSelection = RCC_DCMIPPCLKSOURCE_IC17;
    PeriphClkInitStruct.ICSelection[RCC_IC17].ClockSelection = RCC_ICCLKSOURCE_PLL1;
    PeriphClkInitStruct.ICSelection[RCC_IC17].ClockDivider = 4;

    // CSI (MIPI D-PHY) 使用 IC18 时钟通道，来源于 PLL1，60分频 (根据官方驱动匹配)
    PeriphClkInitStruct.ICSelection[RCC_IC18].ClockSelection = RCC_ICCLKSOURCE_PLL1;
    PeriphClkInitStruct.ICSelection[RCC_IC18].ClockDivider = 60;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* 2. 使能 DCMIPP 和 CSI 模块的时钟 */
    __HAL_RCC_DCMIPP_CLK_ENABLE();
    __HAL_RCC_CSI_CLK_ENABLE();

    /* 3. 硬件强制复位一次，确保模块处于干净的初始状态 */
    __HAL_RCC_DCMIPP_FORCE_RESET();
    __HAL_RCC_DCMIPP_RELEASE_RESET();
    __HAL_RCC_CSI_FORCE_RESET();
    __HAL_RCC_CSI_RELEASE_RESET();

    /* 4. 配置并开启 NVIC 中断 */
    /* 因为你使用了 FreeRTOS，优先级必须 >= 5 (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY) */
    HAL_NVIC_SetPriority(DCMIPP_IRQn, 5, 0);
    //HAL_NVIC_SetPriority(CSI_IRQn, 5, 0);    // MIPI CSI 中断也需要开启

    HAL_NVIC_EnableIRQ(DCMIPP_IRQn);
    //HAL_NVIC_EnableIRQ(CSI_IRQn);

    /* 提示：MIPI 专用引脚 (CSI_D0P/N 等) 不需要像普通 GPIO 一样配置复位模式 */
  }
}

/**
* @brief DCMIPP MSP De-Initialization
*/
void HAL_DCMIPP_MspDeInit(DCMIPP_HandleTypeDef *hdcmipp)
{
  if(hdcmipp->Instance == DCMIPP)
  {
    /* 1. 禁止外设时钟 */
    __HAL_RCC_DCMIPP_CLK_DISABLE();
    __HAL_RCC_CSI_CLK_DISABLE();

    /* 2. 禁止中断 */
    HAL_NVIC_DisableIRQ(DCMIPP_IRQn);
    //HAL_NVIC_DisableIRQ(CSI_IRQn);
  }
}
/* USER CODE END 1 */

