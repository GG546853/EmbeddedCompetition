#ifndef __ENCODER_TASK_H
#define __ENCODER_TASK_H

#include "cmsis_os.h"

extern osThreadId_t Encoder_TaskHandle;
extern const osThreadAttr_t EncoderTask_attributes;

void Encoder_Task(void *argument);

#endif
