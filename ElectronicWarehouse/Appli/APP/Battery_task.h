#ifndef __BATTERY_TASK_H
#define __BATTERY_TASK_H

#include "app_freertos.h"
#include "main.h"

extern osThreadId_t Battery_TaskHandle;
extern const osThreadAttr_t BatteryTask_attributes;

void Battery_Task(void *argument);

#endif
