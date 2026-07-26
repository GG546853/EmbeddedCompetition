#ifndef __AI_TASK_H__
#define __AI_TASK_H__

#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

extern osThreadId_t AITaskHandle;
extern const osThreadAttr_t AITask_attributes;

extern volatile int g_trigger_identify;
extern volatile int g_identify_done;
extern volatile int g_identify_match;
extern char         g_identify_name[16];
extern volatile int g_identify_index;
extern volatile int g_auto_unlock;

#define VOTE_N  5   // K-of-N voting: total frames
#define VOTE_K  2   // K-of-N voting: required passes

extern volatile int g_enroll_progress;
extern volatile int g_enroll_total;
extern volatile int g_enroll_state;
extern char         g_enroll_msg[64];
extern int          g_capture_pending;
extern int          g_model2_enabled;
void ai_start_enroll(const char *name);

void ai_select_face_by_index(int idx);
void ai_refresh_dropdown_users(void);

void AI_Task(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* __AI_TASK_H__ */
