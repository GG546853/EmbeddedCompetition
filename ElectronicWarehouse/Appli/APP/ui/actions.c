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
#include "RGBLED_task.h"
#include "Electromagnet.h"
#include "storage.h"
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

    ui_set_flag(FLOW_GLOBAL_VARIABLE_FLAG_SAVE, 0);
    ui_set_flag(FLOW_GLOBAL_VARIABLE_FLAG_START, 1);

	    // ④ 创建 AI 任务（人脸检测 + 识别）

    g_model2_enabled = 1;  /* Model 2 on for unlock identify */
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

    /* Wait for AI task to complete identify, timeout 2 seconds */
    int timeout = 200;
    while (!g_identify_done && timeout > 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
        timeout--;
    }

    if (g_identify_match) {
        /* Match: stop AI, open electromagnet, enter main */
        if (AITaskHandle != NULL) {
            osThreadTerminate(AITaskHandle);
            AITaskHandle = NULL;
        }
        osThreadNew(ElectromagnetLock_Task, NULL, &ElectromagnetLockTask_attributes);

        strncpy(Cabinet.User, g_identify_name, sizeof(Cabinet.User) - 1);
        Cabinet.User[sizeof(Cabinet.User) - 1] = '\0';
        if (Cabinet.User[0] == '\0') {
            strncpy(Cabinet.User, "suuser", sizeof(Cabinet.User) - 1);
        }
        ui_set_string(FLOW_GLOBAL_VARIABLE_USER, Cabinet.User);

        /* Notify ESP32: face unlock event */
        {
            char evt[32];
            snprintf(evt, sizeof(evt), "%s:Unlock", Cabinet.User);
            uart4_send_face_event(evt);
            printf("[FACE] Unlock event sent: %s\r\n", evt);
        }

        Barcode_TaskHandle = osThreadNew(Barcode_Task, NULL, &BarcodeTask_attributes);

        eez_flow_push_screen(SCREEN_ID_PAGE_MAIN, LV_SCR_LOAD_ANIM_NONE, 0, 0);
    } else {
        /* No match: stop AI, return to lock screen */
        if (AITaskHandle != NULL) {
            osThreadTerminate(AITaskHandle);
            AITaskHandle = NULL;
        }

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

    if (cabinet <= 27) {
        RGBLED_Flash(1u << (27 - cabinet));
    } else {
        Electromagnet_Open((uint8_t)(1u << (cabinet - 28)));
    }

    const char *pc = (cabinet <= 27) ? inventory_item_T[cabinet].pc
                                     : inventory_item_D[cabinet - 28].pc;
    if (history_count >= 32) history_count = 31;
    memmove(&history_list[1], &history_list[0], (size_t)history_count * sizeof(HistoryRecord));
    HistoryRecord *r = &history_list[0];
    get_current_time_str(r->time, sizeof(r->time));
    strncpy(r->action, (qty > 0) ? "put" : "out", sizeof(r->action));
    strncpy(r->pc, pc, sizeof(r->pc));
    strncpy(r->user, Cabinet.User, sizeof(r->user));
    r->cabinet_id = (uint8_t)cabinet;
    r->quantity   = (uint16_t)((qty > 0) ? qty : -qty);
    history_count++;
    storage_mark_inventory_dirty();
    // storage_mark_history_dirty();  // disabled: history SD persistence
    uart4_send_history(r);
    ui_push_history(history_list, history_count);

    ui_push_inventory(inventory_item_T, 28, inventory_item_D, 6);

    ui_set_integer(FLOW_GLOBAL_VARIABLE_DASH_LS,
        ui_get_integer(FLOW_GLOBAL_VARIABLE_DASH_LS) + qty);
}

void action_new_user(lv_event_t * e) {
    (void)e;

    HAL_LTDC_ConfigColorKeying(&hltdc, 0, 1);
    HAL_LTDC_EnableColorKeying(&hltdc, 1);
    HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);

    memset(g_ltdc_ui_framebuf, 0, sizeof(g_ltdc_ui_framebuf));

    eez_flow_push_screen(SCREEN_ID_CAMERA, LV_SCR_LOAD_ANIM_NONE, 0, 0);
    ui_set_flag(FLOW_GLOBAL_VARIABLE_FLAG_SAVE, 1);
    ui_set_flag(FLOW_GLOBAL_VARIABLE_FLAG_START, 0);
    g_model2_enabled = 0;  /* Model 2 off during new user */
    AITaskHandle = osThreadNew(AI_Task, NULL, &AITask_attributes);
}

void action_save_user(lv_event_t * e) {
    (void)e;
    HAL_LTDC_ConfigColorKeying(&hltdc, 0, 1);
    HAL_LTDC_EnableColorKeying(&hltdc, 1);
    HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);

    memset(g_ltdc_ui_framebuf, 0, sizeof(g_ltdc_ui_framebuf));

    eez_flow_push_screen(SCREEN_ID_CAMERA, LV_SCR_LOAD_ANIM_NONE, 0, 0);

    if (g_enroll_state == 1) {
        /* Already enrolling: just capture one more sample */
        g_capture_pending = 1;
    } else {
        static int user_seq = 0;
        char name[16];
        snprintf(name, sizeof(name), "user%02d", user_seq++);
        ai_start_enroll(name);
    }

    g_model2_enabled = 1;  /* Model 2 on for enrollment */
    if (AITaskHandle == NULL) {
        AITaskHandle = osThreadNew(AI_Task, NULL, &AITask_attributes);
    }

    ui_set_flag(FLOW_GLOBAL_VARIABLE_FLAG_SAVE, 1);
    ui_set_flag(FLOW_GLOBAL_VARIABLE_FLAG_START, 0);
}
void action_printer(lv_event_t * e) {

}
void action_close_down(lv_event_t * e)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, 0);//Power up and raise voltage
}
