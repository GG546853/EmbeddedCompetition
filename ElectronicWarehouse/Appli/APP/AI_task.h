#ifndef __AI_TASK_H__
#define __AI_TASK_H__

#include "cmsis_os2.h"

extern osThreadId_t AITaskHandle;
extern const osThreadAttr_t AITask_attributes;

extern volatile int g_trigger_identify;
extern volatile int g_identify_done;
extern volatile int g_identify_match;
extern char         g_identify_name[16];

void AI_Task(void *argument);

#endif /* __AI_TASK_H__ */
