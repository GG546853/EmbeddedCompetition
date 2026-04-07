#ifndef __SENSOR_TASK_H
#define __SENSOR_TASK_H

#include "app_freertos.h"
#include "main.h"

extern osThreadId_t Sensor_TaskHandle;
extern const osThreadAttr_t SensorTask_attributes;

void Sensor_Task(void *argument);

#endif
