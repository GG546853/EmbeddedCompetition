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
#include "Encoder_task.h"
#include "Battery_task.h"

#include <stdio.h>
#include <stdint.h>
#include "imx335.h"
#include "rgblcd.h"
#include "aht10.h"
#include "gt9xxx.h"
#include "semphr.h"
#include "app_types.h"
#include "ui_bridge.h"
#include "storage.h"
#include "sys.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void StorageFlush_Task(void *argument);
static void StatsTask(void *argument);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STATS_MAX_TASKS       32
#define STATS_INTERVAL_MS     1000
#define STATS_START_DELAY_MS  2000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
SystemState Cabinet;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t StorageFlushHandle;
const osThreadAttr_t StorageFlushTask_attributes = {
  .name = "StorageFlush",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512 * 4
};

static osThreadId_t StatsTaskHandle;
static const osThreadAttr_t StatsTask_attributes = {
  .name = "Stats",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 4
};
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
osSemaphoreId_t face_name_sem;
const osSemaphoreAttr_t inventory_sem_attributes = {
  .name = "inventory_sem"
};
const osSemaphoreAttr_t face_name_sem_attributes = {
  .name = "face_name_sem"
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
/* 32-bit CYCCNT software-extended to a monotonic 64-bit cycle counter.
 * CYCCNT wraps every 2^32 / 600 MHz ~= 7.16 s; the tick hook samples it
 * every 1 ms so each raw delta stays far below 2^32 and is unambiguous. */
static volatile uint64_t rt_cycles;
static volatile uint32_t rt_last;

__weak void configureTimerForRunTimeStats(void)
{
  uint32_t primask = __get_PRIMASK();

  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  /* Do not reset CYCCNT: the trace recorder already uses this counter. */

  __disable_irq();
  rt_last = DWT->CYCCNT;
  rt_cycles = 0u;
  __set_PRIMASK(primask);
}

uint64_t getRunTimeCounterValue(void)
{
  uint32_t primask = __get_PRIMASK();
  uint32_t now;
  uint64_t ret;

  __disable_irq();
  now = DWT->CYCCNT;
  rt_cycles += (uint32_t)(now - rt_last);  /* wrap-safe 32-bit delta */
  rt_last = now;
  ret = rt_cycles;
  __set_PRIMASK(primask);

  return ret;
}

void vApplicationTickHook(void)
{
  /* Guarantees wrap detection even if no context switch happens for >7 s. */
  (void)getRunTimeCounterValue();
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
  storage_mutex = osMutexNew(NULL);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	cam_frame_sem = osSemaphoreNew(1, 0, &cam_frame_sem_attributes);
	inventory_sem = osSemaphoreNew(1, 0, &inventory_sem_attributes);
		face_name_sem = osSemaphoreNew(1, 0, &face_name_sem_attributes);
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
  Sensor_TaskHandle = osThreadNew(Sensor_Task, NULL, &SensorTask_attributes);
  LV_TaskHandle = osThreadNew(LVGL_Task, NULL, &LVTask_attributes);
  //AITaskHandle = osThreadNew(AI_Task, NULL, &AITask_attributes);
  //Electromagnet_TaskHandle = osThreadNew(Electromagnet_Task, NULL, &ElectromagnetTask_attributes);
  Barcode_TaskHandle = osThreadNew(Barcode_Task, NULL, &BarcodeTask_attributes);
  //Printer_TaskHandle = osThreadNew(Printer_Task, NULL, &PrinterTask_attributes);
  //Outbound_TaskHandle = osThreadNew(Outbound_Task, NULL, &OutboundTask_attributes);


  Aht10_TaskHandle   = osThreadNew(Aht10_Task, NULL, &Aht10Task_attributes);
  UART4_RxTaskHandle = osThreadNew(UART4_RxTask, NULL, &UART4_RxTask_attributes);
  Encoder_TaskHandle = osThreadNew(Encoder_Task, NULL, &EncoderTask_attributes);
  VL53L1X_TaskHandle = osThreadNew(VL53L1X_Task, NULL, &VL53L1XTask_attributes);
  Battery_TaskHandle = osThreadNew(Battery_Task, NULL, &BatteryTask_attributes);
  StorageFlushHandle = osThreadNew(StorageFlush_Task, NULL, &StorageFlushTask_attributes);
  StatsTaskHandle = osThreadNew(StatsTask, NULL, &StatsTask_attributes);
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
  vTaskDelay(pdMS_TO_TICKS(3500));
  for(;;)
  {
	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,1);
    osDelay(10000);
  }
  /* USER CODE END defaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* Storage flush task — deferred SD NAND writes */
void StorageFlush_Task(void *argument)
{
    vTaskDelay(pdMS_TO_TICKS(5000));  /* wait for UI + data load to settle */
    for (;;) {
        storage_flush();
        vTaskDelay(pdMS_TO_TICKS(2000));  /* flush every 2 s */
    }
}

/* Per-second per-task CPU share, computed from delta counters to stay
 * wrap-safe on the 32-bit DWT cycle counter (7.16 s at 600 MHz). */
static TaskStatus_t statsCur[STATS_MAX_TASKS];
static TaskStatus_t statsPrev[STATS_MAX_TASKS];
static UBaseType_t statsPrevCount;

static void StatsTask(void *argument)
{
    uint64_t prevCycles;
    uint64_t nowCycles;
    uint64_t totalCycles;
    uint32_t winMs;
    UBaseType_t count;
    UBaseType_t i;
    UBaseType_t j;
    uint64_t prevCounter;
    uint64_t delta;
    uint32_t pct;
    uint32_t watermark;
    const char *name;

    (void)argument;

    vTaskDelay(pdMS_TO_TICKS(STATS_START_DELAY_MS));
    statsPrevCount = uxTaskGetSystemState(statsPrev, STATS_MAX_TASKS, &prevCycles);

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(STATS_INTERVAL_MS));

        count = uxTaskGetSystemState(statsCur, STATS_MAX_TASKS, &nowCycles);
        totalCycles = nowCycles - prevCycles;

        if (totalCycles == 0u)
        {
            prevCycles = nowCycles;
            continue;
        }

        winMs = (uint32_t)(totalCycles / (SystemCoreClock / 1000u));
        printf("--- window %u ms ---\r\n", (unsigned int)winMs);

        for (i = 0u; i < count; i++)
        {
            prevCounter = statsCur[i].ulRunTimeCounter;
            for (j = 0u; j < statsPrevCount; j++)
            {
                if (statsPrev[j].xHandle == statsCur[i].xHandle)
                {
                    prevCounter = statsPrev[j].ulRunTimeCounter;
                    break;
                }
            }

            delta = statsCur[i].ulRunTimeCounter - prevCounter;
            pct = (uint32_t)((delta * 100u) / totalCycles);
            watermark = (uint32_t)uxTaskGetStackHighWaterMark(statsCur[i].xHandle)
                        * (uint32_t)sizeof(StackType_t);
            name = statsCur[i].pcTaskName;

            printf("%-16s %3u%%  stack %5u B\r\n",
                   (name != NULL) ? name : "?",
                   (unsigned int)pct,
                   (unsigned int)watermark);
        }

        for (i = 0u; i < count; i++)
        {
            statsPrev[i] = statsCur[i];
        }
        statsPrevCount = count;
        prevCycles = nowCycles;
    }
}

/* USER CODE END Application */

