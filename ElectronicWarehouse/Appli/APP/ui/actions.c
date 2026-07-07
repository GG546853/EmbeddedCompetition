#include "actions.h"
#include "eez-flow.h"
#include "cmsis_os.h"
#include "actions.h"
#include "screens.h"
#include "eez-flow.h"
#include "Sensor_task.h"
#include "AI_task.h"
#include "rgblcd.h"
#include "dma2d.h"
#include "cmsis_os.h"
#include "main.h"
#include <string.h>

extern osThreadId_t Sensor_TaskHandle;
extern const osThreadAttr_t SensorTask_attributes;
extern LTDC_HandleTypeDef hltdc;
extern uint8_t g_ltdc_ui_framebuf[800 * 480 * 2];
osMutexId_t dma2d_mutex;
const osMutexAttr_t dma2d_mutex_attr = { .name = "dma2d_mutex" };


void action_ver(lv_event_t * e) {
	 (void)e;

	    // ① 开 Color Keying，黑色像素直接穿透到 Layer 0
	    HAL_LTDC_ConfigColorKeying(&hltdc, 0, 1);
	    HAL_LTDC_EnableColorKeying(&hltdc, 1);
	    HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);

	    // ② 清 Layer 1 帧缓冲（避免切屏瞬间残留旧数据）
	    memset(g_ltdc_ui_framebuf, 0, sizeof(g_ltdc_ui_framebuf));

	    // ③ 切屏（Sensor_Task 常驻运行，DCMIPP 一直在采集）
	    eez_flow_push_screen(SCREEN_ID_CAMERA, LV_SCR_LOAD_ANIM_NONE, 0, 0);

	    // ④ 创建 AI 任务（人脸检测 + 识别）
	    AITaskHandle = osThreadNew(AI_Task, NULL, &AITask_attributes);
}

void action_action_stop_camera(lv_event_t * e) {

    (void)e;

    // ① 终止 AI 任务
    if (AITaskHandle != NULL) {
        osThreadTerminate(AITaskHandle);
        AITaskHandle = NULL;
    }

    // ② 关 Color Keying（Main 屏幕不能开，否则黑色 UI 元素会消失）
    HAL_LTDC_DisableColorKeying(&hltdc, 1);
    HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);

    // ③ 清除 Layer 0 摄像头残留画面
    rgblcd_clear(BLUE);

    // ④ 返回 Main（Sensor_Task 继续在后台运行，DCMIPP 持续采集）
    eez_flow_pop_screen(LV_SCR_LOAD_ANIM_NONE, 0, 0);
}

void action_action_recognize(lv_event_t * e) {

}



void action_iobutton(lv_event_t * e) {

}


void action_printer(lv_event_t * e) {

}
