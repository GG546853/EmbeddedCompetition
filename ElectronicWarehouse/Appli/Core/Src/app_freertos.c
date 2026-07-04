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
#include "Outbound_task.h"
#include "Printer_task.h"
#include "Aht10_task.h"
#include "tim.h"
#include "UART4_RxTask.h"
#include "Outbound_task.h"

#include <stdio.h>
#include "imx335.h"
#include "rgblcd.h"
#include "aht10.h"
#include "gt9xxx.h"
#include "semphr.h"
#include "app_types.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
SystemState Cabinet;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osSemaphoreId_t cam_frame_sem;
osSemaphoreId_t inventory_sem;//用于协调 Barcode_Task 和 UART4_RxTask 之间的通信
const osSemaphoreAttr_t inventory_sem_attributes = {
  .name = "inventory_sem"
};
const osSemaphoreAttr_t cam_frame_sem_attributes = {
  .name = "cam_frame_sem"
};
osMutexId_t i2c2_mutex;
const osMutexAttr_t i2c2_mutex_attributes = {
  .name = "i2c2_mutex"
};
osMutexId_t flow_var_mutex;
const osMutexAttr_t flow_var_mutex_attributes = {
  .name = "flow_var_mutex"
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
  flow_var_mutex = osMutexNew(&flow_var_mutex_attributes);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	cam_frame_sem = osSemaphoreNew(1, 0, &cam_frame_sem_attributes);
	inventory_sem = osSemaphoreNew(1, 0, &inventory_sem_attributes);
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

  //RGBLED_TaskHandle = osThreadNew(RGBLED_Task, (void *)(uintptr_t)0, &RGBLEDTask_attributes);
  //Sensor_TaskHandle = osThreadNew(Sensor_Task, NULL, &SensorTask_attributes);
  LV_TaskHandle = osThreadNew(LVGL_Task, NULL, &LVTask_attributes);
  //AITaskHandle = osThreadNew(AI_Task, NULL, &AITask_attributes);
  //Electromagnet_TaskHandle = osThreadNew(Electromagnet_Task, NULL, &ElectromagnetTask_attributes);
  Barcode_TaskHandle = osThreadNew(Barcode_Task, NULL, &BarcodeTask_attributes);
  //Printer_TaskHandle = osThreadNew(Printer_Task, NULL, &PrinterTask_attributes);
  //Outbound_TaskHandle = osThreadNew(Outbound_Task, NULL, &OutboundTask_attributes);


  //Aht10_TaskHandle = osThreadNew(Aht10_Task, NULL, &Aht10Task_attributes);
  UART4_RxTaskHandle = osThreadNew(UART4_RxTask, NULL, &UART4_RxTask_attributes);
  //VL53L1X_TaskHandle = osThreadNew(VL53L1X_Task, NULL, &VL53L1XTask_attributes);
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
	strcpy(Cabinet.User, "ZS");
	//char now[32];
	//osThreadNew(RGBLED_Task, (void *)(uintptr_t)0xA800000, &RGBLEDTask_attributes);
	//osThreadNew(Electromagnet_Task, (void *)(uintptr_t)0x30, &ElectromagnetTask_attributes);
  for(;;)
  {
//    printf("[PC] T: ");
//    for (int i = 0; i < 28; i++) {
//        if (inventory_item_T[i].pc[0] == '\0')
//            printf("0 ");
//        else
//            printf("%s ", inventory_item_T[i].pc);
//    }
//    printf("| D: ");
//    for (int i = 0; i < 6; i++) {
//        if (inventory_item_D[i].pc[0] == '\0')
//            printf("0 ");
//        else
//            printf("%s ", inventory_item_D[i].pc);
//    }
//    printf("\r\n");

    osDelay(2000);
  }
  /* USER CODE END defaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

