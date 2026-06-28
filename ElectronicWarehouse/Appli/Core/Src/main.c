/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os2.h"
#include "cacheaxi.h"
#include "csi.h"
#include "dcmipp.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "ramcfg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "xspi.h"
#include "xspim.h"
#include "gpio.h"
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sys.h"
#include "hyperram.h"
#include "norflash.h"
#include "imx335.h"
#include "rgblcd.h"
#include "uart.h"
#include "aht10.h"
#include "../Drivers/BSP/SoftI2C/soft_i2c.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#ifdef DEBUG
static HyperRAM_ObjectTypeDef HyperRAMObject = {0}; //句柄
static NORFlash_ObjectTypeDef NORFlashObject = {0};
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void MX_FREERTOS_Init(void);
static void SystemIsolation_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	//SCB->VTOR = 0x34000400;
#ifdef DEBUG
    // Debug 模式：通常指向 RAM 中的向量表
    SCB->VTOR = 0x34000400;
#else
    // Release 模式：指向外部 Flash 的起始地址
    SCB->VTOR = 0x70100400;
#endif

	SystemCoreClockUpdate();


//	  /* Enable I-Cache---------------------------------------------------------*/
//	  SCB_EnableICache();
//
//	  /* Enable D-Cache---------------------------------------------------------*/
//	  SCB_EnableDCache();

//	MPU_Region_InitTypeDef MPU_InitStruct = {0};
//	  MPU_Attributes_InitTypeDef MPU_AttributesInit = {0};
//
//	  /* 1. 禁用 MPU */
//	  HAL_MPU_Disable();
//
//	  /* 2. 配置内存属性集 0 (配置为：Normal Memory, Non-Cacheable 非缓存)
//	     用以替代旧版的 TypeExtField、IsCacheable、IsBufferable 组合 */
//	  MPU_AttributesInit.Number = MPU_ATTRIBUTES_NUMBER0;
//	  MPU_AttributesInit.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
//	  HAL_MPU_ConfigMemoryAttributes(&MPU_AttributesInit);
//
//	  /* 3. 配置 MPU 保护区域 0 */
//	  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
//	  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
//
//	  /* 区域起始地址 */
//	  MPU_InitStruct.BaseAddress = 0x90000000;
//
//	  /* 区域结束地址（LimitAddress）
//	     32MB 空间大小在 16 进制下为 0x02000000 字节。
//	     LimitAddress = BaseAddress + Size - 1 = 0x90000000 + 0x02000000 - 1 = 0x91FFFFFF */
//	  MPU_InitStruct.LimitAddress = 0x91FFFFFF;
//
//	  /* 绑定前面配置的内存属性集 0 */
//	  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
//
//	  /* 读写访问权限 (对应旧版 MPU_REGION_FULL_ACCESS) */
//	  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
//
//	  /* 允许非特权级与特权级软件在该区域执行指令 */
//	  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
//	  MPU_InitStruct.DisablePrivExec = MPU_PRIV_INSTRUCTION_ACCESS_ENABLE;
//
//	  /* 非共享属性 */
//	  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
//
//	  /* 应用 MPU 区域配置 */
//	  HAL_MPU_ConfigRegion(&MPU_InitStruct);
//
//	  /* 4. 启用 MPU */
//	  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();

  /* USER CODE BEGIN Init */
#ifdef DEBUG
  sys_clock_config_debug();
#endif
  SystemCoreClockUpdate();
  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DCMIPP_Init();
  MX_DMA2D_Init();
  MX_I2C2_Init();
  MX_I2C4_Init();
  //MX_XSPI1_Init();
  MX_LTDC_Init();
  MX_USART1_UART_Init();
  //MX_XSPI2_Init();
  MX_CACHEAXI_Init();
  MX_RAMCFG_Init();
  MX_UART7_Init();
  MX_SPI5_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_X_CUBE_AI_Init();
  SystemIsolation_Config();
  /* USER CODE BEGIN 2 */

//  setvbuf(stdin, NULL, _IONBF, 0);
//  setvbuf(stdout, NULL, _IONBF, 0);
  uart_init(115200);

#ifdef DEBUG
  MX_XSPI1_Init();
  if (HyperRAM_Init(&HyperRAMObject, &hxspi1) != HyperRAM_OK)
  {
      Error_Handler();
  }
  HyperRAM_EnableMemoryMappedMode(&HyperRAMObject);

  MX_XSPI2_Init();
  uint32_t xspi2_clk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI2);
  if (NORFlash_Init(&NORFlashObject, &hxspi2, xspi2_clk) != NORFlash_OK)
    {
        Error_Handler();
    }
  if (NORFlash_EnableMemoryMappedMode(&NORFlashObject) != NORFlash_OK)
  {
      Error_Handler();
  }
#endif

//  /* ==== AHT10 软件 I2C 诊断 ==== */
//  printf("AHT10 Soft I2C Diagnostic:\r\n");
//  printf("  SystemCoreClock=%lu\r\n", SystemCoreClock);
//
//  /* 查看 GPIOH 初始状态 */
//  printf("  GPIOH MODER=0x%08lX ODR=0x%08lX IDR=0x%08lX\r\n",
//         GPIOH->MODER, GPIOH->ODR, GPIOH->IDR);
//
//  /* 初始化软 I2C（重配 PH7/PH8 为开漏输出） */
//  soft_i2c_init();
//
//  printf("  After soft_i2c_init:\r\n");
//  printf("  GPIOH MODER=0x%08lX ODR=0x%08lX IDR=0x%08lX\r\n",
//         GPIOH->MODER, GPIOH->ODR, GPIOH->IDR);
//
//  /* 手动翻转 SDA 验证 GPIO 控制有效 */
//  printf("  Toggling SDA LOW...\r\n");
//  SDA_L();
//  HAL_Delay(500);
//  printf("  GPIOH ODR=0x%08lX IDR=0x%08lX\r\n", GPIOH->ODR, GPIOH->IDR);
//
//  printf("  Toggling SDA HIGH...\r\n");
//  SDA_H();
//  HAL_Delay(500);
//  printf("  GPIOH ODR=0x%08lX IDR=0x%08lX\r\n", GPIOH->ODR, GPIOH->IDR);
//
//  /* 复位总线后尝试 AHT10 */
//  SDA_H();
//  SCL_H();
//  HAL_Delay(10);
//
//  printf("  Calling aht10_init...\r\n");
//  fflush(stdout);
//
//  if (aht10_init() != 0) {
//      printf("  Init FAILED\r\n");
//      printf("  Final GPIOH MODER=0x%08lX ODR=0x%08lX IDR=0x%08lX\r\n",
//             GPIOH->MODER, GPIOH->ODR, GPIOH->IDR);
//      while (1) { HAL_Delay(1000); }
//  }
//  printf("  Init OK\r\n");
//
//  float t = 0, h = 0;
//  if (aht10_read(&h, &t) != 0) {
//      printf("  Read FAILED\r\n");
//      while (1) { HAL_Delay(1000); }
//  }
//  printf("  T=%.1f C  H=%.1f %%\r\n", t, h);
//  fflush(stdout);


  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Call init function for freertos objects (in app_freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_TIM|RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPCLKSOURCE_HSI;
  PeriphClkInitStruct.TIMPresSelection = RCC_TIMPRES_DIV1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RIF Initialization Function
  * @param None
  * @retval None
  */
  static void SystemIsolation_Config(void)
{

/* USER CODE BEGIN RIF_Init 0 */

/* USER CODE END RIF_Init 0 */

  /* set all required IPs as secure privileged */
  __HAL_RCC_RIFSC_CLK_ENABLE();

  /*RIMC configuration*/
  RIMC_MasterConfig_t RIMC_master = {0};
  RIMC_master.MasterCID = RIF_CID_1;
  RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DCMIPP, &RIMC_master);

  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DMA2D, &RIMC_master);

  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC1, &RIMC_master);

  RIMC_master.MasterCID = RIF_CID_1;
  RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC2, &RIMC_master);

  /*RISUP configuration*/
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_TIM6 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DCMIPP , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DMA2D , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL1 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL2 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);

  /* RIF-Aware IPs Config */

  /* set up GPIO configuration */
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_2,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_3,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_5,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_8,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_9,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_10,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_11,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_10,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_11,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_12,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_13,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_15,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOC,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOC,GPIO_PIN_6,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOC,GPIO_PIN_10,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOC,GPIO_PIN_11,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOC,GPIO_PIN_12,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOC,GPIO_PIN_13,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOD,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOD,GPIO_PIN_4,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOD,GPIO_PIN_5,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOD,GPIO_PIN_14,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_5,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_7,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_10,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_13,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_14,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOF,GPIO_PIN_8,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOF,GPIO_PIN_9,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOF,GPIO_PIN_12,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOF,GPIO_PIN_13,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOF,GPIO_PIN_14,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOG,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOG,GPIO_PIN_4,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOG,GPIO_PIN_6,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOG,GPIO_PIN_9,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOG,GPIO_PIN_10,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOG,GPIO_PIN_11,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOG,GPIO_PIN_13,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOG,GPIO_PIN_14,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOH,GPIO_PIN_2,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOH,GPIO_PIN_3,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOH,GPIO_PIN_4,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOH,GPIO_PIN_5,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOH,GPIO_PIN_7,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOH,GPIO_PIN_8,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOH,GPIO_PIN_9,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_2,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_3,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_4,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_5,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_6,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_8,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_9,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_10,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPION,GPIO_PIN_11,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOO,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOO,GPIO_PIN_2,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOO,GPIO_PIN_4,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOO,GPIO_PIN_5,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP,GPIO_PIN_2,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP,GPIO_PIN_3,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP,GPIO_PIN_4,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP,GPIO_PIN_5,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP,GPIO_PIN_6,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOP,GPIO_PIN_7,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOQ,GPIO_PIN_3,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOQ,GPIO_PIN_4,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOQ,GPIO_PIN_5,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOQ,GPIO_PIN_6,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOQ,GPIO_PIN_7,GPIO_PIN_SEC|GPIO_PIN_NPRIV);

/* USER CODE BEGIN RIF_Init 1 */
  RIMC_master.MasterCID = RIF_CID_1;
  RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_NPU, &RIMC_master);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_NPU, RIF_ATTRIBUTE_PRIV | RIF_ATTRIBUTE_SEC);
/* USER CODE END RIF_Init 1 */
/* USER CODE BEGIN RIF_Init 2 */

/* USER CODE END RIF_Init 2 */

}

/* USER CODE BEGIN 4 */
  extern osSemaphoreId_t cam_frame_sem;
  void HAL_DCMIPP_PIPE_FrameEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe)
  {
    imx335_dcmipp_pipe_frame_cb(hdcmipp, Pipe);
    if (Pipe == DCMIPP_PIPE2)
    {
        // 释放信号量，通知 AI 任务“图像写完了，可以开始推理了！”
        if (cam_frame_sem != NULL) {
            osSemaphoreRelease(cam_frame_sem);
        }
    }
    __NOP();
  }

  void HAL_DCMIPP_PIPE_VsyncEventCallback(DCMIPP_HandleTypeDef *hdcmipp, uint32_t Pipe)
  {
    imx335_dcmipp_pipe_vsync_cb(hdcmipp, Pipe);
    __NOP();
  }

  void HAL_LTDC_ErrorCallback(LTDC_HandleTypeDef *hltdc)
  {      /* 🔴 在这里也打个红色的断点 */
//      uint32_t error_code = hltdc->ErrorCode;
      __NOP();
  }

  void HAL_DCMIPP_ErrorCallback(DCMIPP_HandleTypeDef *hdcmipp)
  {      /* 🔴 在这里打个红色的断点 */
//      uint32_t error_code = hdcmipp->ErrorCode;  // 加一行这个方便观察
      __NOP();
  }


  int _close(int file) { return -1; }
  int _fstat(int file, void *st) { return 0; }
  int _isatty(int file) { return 1; }
  int _lseek(int file, int ptr, int dir) { return 0; }
  int _read(int file, char *ptr, int len)
  {
      int i;
      for (i = 0; i < len; i++)
      {
          // 1. 每次接收前清除可能存在的溢出错误标志，防止 UART 卡死
          __HAL_UART_CLEAR_OREFLAG(&huart1);

          // 2. 逐字节接收数据（阻塞等待）
          if (HAL_UART_Receive(&huart1, (uint8_t *)&ptr[i], 1, HAL_MAX_DELAY) != HAL_OK)
          {
              break; // 接收出错或超时则退出
          }

          // 3. 终端回显：将接收到的字符原样发送回电脑（让用户能在串口助手看到自己输入的内容）
          HAL_UART_Transmit(&huart1, (uint8_t *)&ptr[i], 1, 0xFFFF);

          // 4. 处理回车换行逻辑
          if (ptr[i] == '\r' || ptr[i] == '\n')
          {
              ptr[i] = '\n'; // C 标准库解析通常以 '\n' 结束输入

              // 在回显时补发一个 '\r' 换行符，以便终端工具正确换行到行首
              char extra_r = '\r';
              HAL_UART_Transmit(&huart1, (uint8_t *)&extra_r, 1, 0xFFFF);

              i++;
              break; // 读取到换行，结束当前读取
          }
      }

      // 5. 必须返回实际接收到的字节数，而不是写死 len
      return i;
  }
  int _write(int file, char *ptr, int len)
  {
      // 将 printf 的数据通过串口 1 发送出去
      HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, 0xFFFF);
      return len;
  }
  int _getpid(void) { return 1; }
  int _kill(int pid, int sig) { return -1; }
  int _open(char *file, int flags, int mode) { return -1; }

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
