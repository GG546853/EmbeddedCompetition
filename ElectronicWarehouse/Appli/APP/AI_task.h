#ifndef __AI_TASK_H
#define __AI_TASK_H

#include "cmsis_os2.h"

extern osThreadId_t AITaskHandle;
extern const osThreadAttr_t AITask_attributes;

void StartAITask(void *argument);

#endif /* __AI_TASK_H */
