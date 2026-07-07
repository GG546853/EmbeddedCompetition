#include "actions.h"
#include "eez-flow.h"
#include "cmsis_os.h"
#include "actions.h"
#include "screens.h"
#include "eez-flow.h"
#include "Sensor_task.h"
#include "AI_task.h"
#include "Barcode_task.h"
#include "Outbound_task.h"
#include "rgblcd.h"
#include "dma2d.h"
#include "cmsis_os.h"
#include "main.h"
#include "app_types.h"
#include "vars.h"
#include "ui_bridge.h"
#include "UART_protocol.h"
#include <string.h>
#include <stdlib.h>

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
    (void)e;

    g_identify_done  = 0;
    g_identify_match = 0;
    g_trigger_identify = 1;

    /* 等待 AI 任务完成识别，超时 2 秒 */
    int timeout = 200;
    while (!g_identify_done && timeout > 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
        timeout--;
    }

    if (g_identify_match) {
        /* 匹配成功：关 AI + 摄像头，开扫码，进主页 */
        if (AITaskHandle != NULL) {
            osThreadTerminate(AITaskHandle);
            AITaskHandle = NULL;
        }
        if (Sensor_TaskHandle != NULL) {
            osThreadTerminate(Sensor_TaskHandle);
            Sensor_TaskHandle = NULL;
        }

        HAL_LTDC_DisableColorKeying(&hltdc, 1);
        HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);
        rgblcd_clear(BLUE);

        strncpy(Cabinet.User, g_identify_name, sizeof(Cabinet.User) - 1);
        Cabinet.User[sizeof(Cabinet.User) - 1] = '\0';

        Barcode_TaskHandle = osThreadNew(Barcode_Task, NULL, &BarcodeTask_attributes);

        eez_flow_push_screen(SCREEN_ID_PAGE_MAIN, LV_SCR_LOAD_ANIM_NONE, 0, 0);
    } else {
        /* 未匹配：关 AI，返回锁屏 */
        if (AITaskHandle != NULL) {
            osThreadTerminate(AITaskHandle);
            AITaskHandle = NULL;
        }

        HAL_LTDC_DisableColorKeying(&hltdc, 1);
        HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);
        rgblcd_clear(BLUE);

        eez_flow_push_screen(SCREEN_ID_PAGE_LOCK, LV_SCR_LOAD_ANIM_NONE, 0, 0);
    }
}



void action_iobutton(lv_event_t * e) {
    (void)e;

    int cabinet = ui_get_integer(FLOW_GLOBAL_VARIABLE_SELECTED_CABINET);
    if (cabinet == -1) return;

    if (cabinet <= 27) {
        if (inventory_item_T[cabinet].pc[0] == '\0') return;
    } else {
        if (inventory_item_D[cabinet - 28].pc[0] == '\0') return;
    }

    int qty = ui_get_integer(FLOW_GLOBAL_VARIABLE_OUTBOUND_QTY);

    int cur_qty = (cabinet <= 27) ? inventory_item_T[cabinet].quantity
                                  : inventory_item_D[cabinet - 28].quantity;
    if (cur_qty + qty < 0) return;

    if (cabinet <= 27) {
        inventory_item_T[cabinet].quantity += qty;
    } else {
        inventory_item_D[cabinet - 28].quantity += qty;
    }

    char loc[4];
    if (cabinet <= 27) {
        sprintf(loc, "T%02d", cabinet);
    } else {
        sprintf(loc, "D%02d", cabinet - 28);
    }
    if (qty > 0) {
        uart4_report_inbound(loc, (uint16_t)qty);
    } else {
        uart4_report_outbound(loc, (uint16_t)(-qty));
    }

    const char *pc = (cabinet <= 27) ? inventory_item_T[cabinet].pc
                                     : inventory_item_D[cabinet - 28].pc;
    HistoryRecord *r = &history_list[history_count];
    get_current_time_str(r->time, sizeof(r->time));
    strncpy(r->action, (qty > 0) ? "put" : "out", sizeof(r->action));
    strncpy(r->pc, pc, sizeof(r->pc));
    strncpy(r->user, Cabinet.User, sizeof(r->user));
    r->cabinet_id = (uint8_t)cabinet;
    r->quantity   = (uint16_t)((qty > 0) ? qty : -qty);
    history_count++;
    uart4_send_history(r);
    ui_push_history(history_list, history_count);

    ui_push_inventory(inventory_item_T, 28, inventory_item_D, 6);
}


void action_printer(lv_event_t * e) {

}
