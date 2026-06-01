#ifndef __AI_TASK_H__
#define __AI_TASK_H__

#include "cmsis_os2.h"

extern osThreadId_t AITaskHandle;
extern const osThreadAttr_t AITask_attributes;

void AI_Task(void *argument);

#endif /* __AI_TASK_H__ */
