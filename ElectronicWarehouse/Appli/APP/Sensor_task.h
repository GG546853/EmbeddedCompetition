#ifndef __SENSOR_TASK_H
#define __SENSOR_TASK_H

#include "app_freertos.h"
#include "main.h"


extern osThreadId_t Sensor_TaskHandle;
extern const osThreadAttr_t SensorTask_attributes;

void Sensor_Task(void *argument);
void Preprocess_Camera_Image(uint16_t *src_buf, int8_t *dst_buf);
void LCD_Draw_Rect(uint16_t *framebuf, int x, int y, int w, int h, uint16_t color);

#endif
