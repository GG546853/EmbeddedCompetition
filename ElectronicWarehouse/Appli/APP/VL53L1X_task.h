#ifndef __VL53L1X_TASK_H
#define __VL53L1X_TASK_H

#include "app_freertos.h"
#include "main.h"

extern osThreadId_t VL53L1X_TaskHandle;
extern const osThreadAttr_t VL53L1XTask_attributes;

void VL53L1X_Task(void *argument);

#endif
