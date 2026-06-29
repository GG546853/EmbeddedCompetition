/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
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
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "RGBLED_task.h"
#include "Sensor_task.h"
#include "Electromagnet.h"
#include "AI_task.h"
#include "LV_task.h"
#include "VL53L1X_task.h"
#include "Barcode_task.h"
#include "Printer_task.h"
#include "tim.h"

#include <stdio.h>
#include "imx335.h"
#include "rgblcd.h"
#include "aht10.h"
#include "gt9xxx.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osSemaphoreId_t cam_frame_sem;
const osSemaphoreAttr_t cam_frame_sem_attributes = {
  .name = "cam_frame_sem"
};
osMutexId_t i2c2_mutex;
const osMutexAttr_t i2c2_mutex_attributes = {
  .name = "i2c2_mutex"
};
/* USER CODE END FunctionPrototypes */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  i2c2_mutex = osMutexNew(&i2c2_mutex_attributes);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	cam_frame_sem = osSemaphoreNew(1, 0, &cam_frame_sem_attributes);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  //RGBLED_TaskHandle = osThreadNew(RGBLED_Task, NULL, &RGBLEDTask_attributes);
  //Sensor_TaskHandle = osThreadNew(Sensor_Task, NULL, &SensorTask_attributes);
  //LV_TaskHandle = osThreadNew(LVGL_Task, NULL, &LVTask_attributes);
  //AITaskHandle = osThreadNew(AI_Task, NULL, &AITask_attributes);
  //Electromagnet_TaskHandle = osThreadNew(Electromagnet_Task, NULL, &ElectromagnetTask_attributes);
  //VL53L1X_TaskHandle = osThreadNew(VL53L1X_Task, NULL, &VL53L1XTask_attributes);
  //Barcode_TaskHandle = osThreadNew(Barcode_Task, NULL, &BarcodeTask_attributes);
  //Printer_TaskHandle = osThreadNew(Printer_Task, NULL, &PrinterTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */

  const uint8_t cfg_1txt[184] = {
    0x60,0xE0,0x01,0x10,0x01,0x05,0x0F,0x00,0x01,0x08,
    0x28,0x05,0x50,0x32,0x03,0x05,0x00,0x00,0xFF,0xFF,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89,0x28,0x0A,
    0x17,0x15,0x31,0x0D,0x00,0x00,0x02,0x9B,0x03,0x25,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,
    0x00,0x0F,0x94,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,
    0x8D,0x13,0x00,0x5C,0x1E,0x00,0x3C,0x30,0x00,0x29,
    0x4C,0x00,0x1E,0x78,0x00,0x1E,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x08,0x0A,0x0C,0x0E,0x10,0x12,0x14,0x16,
    0x18,0x1A,0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0x00,0x02,0x04,0x05,0x06,0x08,0x0A,0x0C,
    0x0E,0x1D,0x1E,0x1F,0x20,0x22,0x24,0x28,0x29,0xFF,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF
  };
  uint8_t buf[184];
  uint8_t data;
  uint16_t sum = 0;
  uint8_t checksum;

  osDelay(500);

  if (gt9xxx_init() != 0) {
    printf("GT9xxx init failed!\r\n");
  } else {
//    /* ====== 写入 1.txt 配置表 ====== */
//    printf("Writing config from 1.txt...\r\n");
//
//    /* 先停扫描，进入配置模式 */
//    data = 0x00;
//    gt9xxx_wr_reg(GT9XXX_CTRL_REG, &data, 1);
//    osDelay(10);
//
//    /* 写配置 + checksum */
//    gt9xxx_wr_reg(GT9XXX_CFGS_REG, (uint8_t *)cfg_1txt, 184);
//    for (int i = 0; i < 184; i++) sum += cfg_1txt[i];
//    checksum = (0x100 - (sum & 0xFF)) & 0xFF;
//    gt9xxx_wr_reg(GT9XXX_CHECK_REG, &checksum, 1);
//    printf("Checksum: 0x%02X\r\n", checksum);
//
//    /* 重启扫描 → 芯片检测 checksum 变化，自动烧 flash */
//    data = 0x01;
//    gt9xxx_wr_reg(GT9XXX_CTRL_REG, &data, 1);
//    osDelay(200);
//
//    /* 软复位验证：从 flash 重新加载 */
//    data = 0x02;
//    gt9xxx_wr_reg(GT9XXX_CTRL_REG, &data, 1);
//    osDelay(10);
//    data = 0x00;
//    gt9xxx_wr_reg(GT9XXX_CTRL_REG, &data, 1);

    /* ====== 回读验证 ====== */
    gt9xxx_rd_reg(GT9XXX_CFGS_REG, buf, 184);

    printf("\r\n=== GT9xxx Config Table After Write ===\r\n");
    for (int i = 0; i < 184; i += 8) {
      printf("[0x%04X]: ", 0x8047 + i);
      for (int j = 0; j < 8 && (i + j) < 184; j++) {
        printf("0x%02X ", buf[i + j]);
      }
      printf("\r\n");
    }

    /* 逐字节对比 */
    int mismatch = 0;
    for (int i = 0; i < 184; i++) {
      if (buf[i] != cfg_1txt[i]) {
        if (mismatch == 0) printf("\r\n=== MISMATCH ===\r\n");
        printf("  [0x%04X]: wrote 0x%02X, read 0x%02X\r\n",
               0x8047 + i, cfg_1txt[i], buf[i]);
        mismatch++;
      }
    }
    if (mismatch == 0) {
      printf("\r\n=== All 184 bytes match! ===\r\n");
    } else {
      printf("=== %d bytes mismatch ===\r\n", mismatch);
    }
  }

  /* Infinite loop */
  for(;;)
  {
    osDelay(2000);
  }
  /* USER CODE END defaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

