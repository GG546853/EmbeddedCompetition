#include "cmsis_os.h"
#include "eez-flow.h"
#include "vars.h"
#include "structs.h"
#include "ui_bridge.h"
#include "screens.h"
#include "AI_task.h"
extern "C" {
#include "Electromagnet.h"
#include "UART_protocol.h"
#include "Barcode_task.h"
}
extern "C" {
#include "app_types.h"
}
extern int history_count;
extern HistoryRecord history_list[32];

extern osMutexId_t flow_var_mutex;

struct _lv_event_t;
typedef struct _lv_event_t lv_event_t;
extern "C" void get_current_time_str(char *buf, int bufsize);

void ui_set_integer(int var_id, int value) {
    eez::flow::setGlobalVariable(var_id, eez::Value(value));
}

int ui_get_integer(int var_id) {
    return eez::flow::getGlobalVariable(var_id).getInt();
}

void ui_set_string(int var_id, const char *value) {
    eez::flow::setGlobalVariable(var_id, eez::Value(value));
}

const char *ui_get_string(int var_id) {
    return eez::flow::getGlobalVariable(var_id).getString();
}

static void copy_item_to_cab(CabinetValue &cab, const InventoryItem *item) {
    cab.name(item->name);
    cab.qty(item->quantity);
    cab.pa(item->pa);
    cab.spec(item->type);
    cab.pc(item->pc);
}

static void copy_cab_to_item(InventoryItem *item, CabinetValue cab) {
    strncpy(item->name, cab.name(), sizeof(item->name) - 1);
    item->quantity = (uint16_t)cab.qty();
    strncpy(item->pa,   cab.pa(),   sizeof(item->pa) - 1);
    strncpy(item->type, cab.spec(), sizeof(item->type) - 1);
    strncpy(item->pc,   cab.pc(),   sizeof(item->pc) - 1);
}

void ui_push_inventory(const InventoryItem *t_items, int t_count,
                       const InventoryItem *d_items, int d_count) {
    int total = t_count + d_count;
    ArrayOfCabinetValue cabinets(total);
    for (int i = 0; i < t_count; i++) {
        CabinetValue cab;
        copy_item_to_cab(cab, &t_items[i]);
        cabinets.at(i, cab);
    }
    for (int i = 0; i < d_count; i++) {
        CabinetValue cab;
        copy_item_to_cab(cab, &d_items[i]);
        cabinets.at(t_count + i, cab);
    }
    eez::flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_CABINETS, cabinets);
    eez::flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_CABINET_COUNT, eez::Value(total));
}

void ui_pull_inventory(InventoryItem *t_items, int t_count,
                       InventoryItem *d_items, int d_count) {
    Value v = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_CABINETS);
    if (!v.isArray()) {
        return;
    }
    ArrayOfCabinetValue cabinets(v);
    int n = (int)cabinets.size();
    for (int i = 0; i < t_count && i < n; i++) {
        copy_cab_to_item(&t_items[i], cabinets.at(i));
    }
    for (int i = 0; i < d_count && (t_count + i) < n; i++) {
        copy_cab_to_item(&d_items[i], cabinets.at(t_count + i));
    }
}

void ui_push_history(const HistoryRecord *list, int count) {
    ArrayOfLogEntryValue logs(count);
    for (int i = 0; i < count; i++) {
        LogEntryValue entry;
        entry.time(list[i].time);
        entry.user(list[i].user);
        entry.pc(list[i].pc);
        entry.qty((int)list[i].quantity);
        entry.action(list[i].action);
        entry.cabinet_id((int)list[i].cabinet_id);
        logs.at(i, entry);
    }
    eez::flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_HISTORY_RECORDS, logs);
}

int ui_pull_history(HistoryRecord *list, int max_count) {
    Value v = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_HISTORY_RECORDS);
    if (!v.isArray()) {
        return 0;
    }
    ArrayOfLogEntryValue logs(v);
    int n = (int)logs.size();
    if (n > max_count) n = max_count;
    for (int i = 0; i < n; i++) {
        LogEntryValue entry = logs.at(i);
        strncpy(list[i].time,   entry.time(),   sizeof(list[i].time) - 1);
        strncpy(list[i].user,   entry.user(),   sizeof(list[i].user) - 1);
        strncpy(list[i].pc,     entry.pc(),     sizeof(list[i].pc) - 1);
        list[i].quantity   = (uint16_t)entry.qty();
        strncpy(list[i].action, entry.action(), sizeof(list[i].action) - 1);
        list[i].cabinet_id = (uint8_t)entry.cabinet_id();
    }
    return n;
}

int ui_get_log_count(void) {
    Value v = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_HISTORY_RECORDS);
    int count = 0;
    if (v.isArray()) count = (int)v.getArray()->arraySize;
    return count;
}

void ui_get_log_entry(int index, char *time, char *user, char *pc,
                      int *qty, char *action, int *cab_id) {
    Value v = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_HISTORY_RECORDS);
    if (!v.isArray()) {
        return;
    }
    ArrayOfLogEntryValue logs(v);
    if (index >= (int)logs.size()) {
        return;
    }
    LogEntryValue entry = logs.at(index);
    strcpy(time,   entry.time());
    strcpy(user,   entry.user());
    strcpy(pc,     entry.pc());
    *qty   = entry.qty();
    strcpy(action, entry.action());
    *cab_id = entry.cabinet_id();
}

void ui_push_alarm(const AlarmRecord *list, int count) {
    osMutexAcquire(flow_var_mutex, osWaitForever);
    ArrayOfLSWValue lsws(count);
    for (int i = 0; i < count; i++) {
        LSWValue entry;
        entry.name(list[i].pc);
        entry.CNUM((int)list[i].cabinet_id);
        entry.STOCK((int)list[i].quantity);
        lsws.at(i, entry);
    }
    eez::flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_LSW, lsws);
    osMutexRelease(flow_var_mutex);
}

extern "C" void action_givetime(lv_event_t * e)
{
    (void)e;
}

static lv_timer_t *cam_text_timer = NULL;

static void cam_text_clear_cb(lv_timer_t *timer) {
    ui_set_string(FLOW_GLOBAL_VARIABLE_CAM_TEXT, "");
    cam_text_timer = NULL;
    lv_timer_delete(timer);
}

extern "C" void ui_set_cam_text(const char *msg) {
    if (cam_text_timer != NULL) {
        lv_timer_delete(cam_text_timer);
        cam_text_timer = NULL;
    }
    ui_set_string(FLOW_GLOBAL_VARIABLE_CAM_TEXT, msg);
    cam_text_timer = lv_timer_create(cam_text_clear_cb, 5000, NULL);
}

extern "C" void ui_set_flag(int var_id, int val) {
    ui_set_integer(var_id, val);
}

extern "C" void ui_poll_enroll_status(void) {
    extern volatile int g_enroll_state;
    extern char g_enroll_msg[64];
    static int last_state = -1;
    static char last_msg[64];

    if (g_enroll_state == 3) {
        /* waiting for ESP32 name — keep refreshing so 5s timer never expires */
        last_state = 3;
        ui_set_cam_text(g_enroll_msg);
        return;
    }

    if (g_enroll_state == 2 && last_state == 3) {
        /* waiting → done: stop AI, navigate back to lock screen */
        extern osThreadId_t AITaskHandle;
        if (AITaskHandle != NULL) {
            osThreadTerminate(AITaskHandle);
            AITaskHandle = NULL;
        }
        eez_flow_push_screen(SCREEN_ID_PAGE_LOCK, LV_SCR_LOAD_ANIM_NONE, 0, 0);
        ai_refresh_dropdown_users();
        ui_set_flag(FLOW_GLOBAL_VARIABLE_FLAG_SAVE, 0);
        ui_set_flag(FLOW_GLOBAL_VARIABLE_FLAG_START, 1);
    }

    if (g_enroll_state != last_state ||
        strncmp(g_enroll_msg, last_msg, sizeof(last_msg)) != 0) {
        last_state = g_enroll_state;
        strncpy(last_msg, g_enroll_msg, sizeof(last_msg) - 1);
        last_msg[sizeof(last_msg) - 1] = '\0';
        if (g_enroll_msg[0] != '\0') {
            ui_set_cam_text(g_enroll_msg);
        }
    }
}

extern "C" void ui_poll_auto_unlock(void) {
    extern volatile int g_auto_unlock;
    extern volatile int g_identify_match;
    extern char g_identify_name[16];
    extern osThreadId_t AITaskHandle;

    if (!g_auto_unlock) return;
    g_auto_unlock = 0;

    /* Stop AI task */
    if (AITaskHandle != NULL) {
        osThreadTerminate(AITaskHandle);
        AITaskHandle = NULL;
    }

    /* Open lock */
    osThreadNew(ElectromagnetLock_Task, NULL, &ElectromagnetLockTask_attributes);

    /* Set user */
    strncpy(Cabinet.User, g_identify_name, sizeof(Cabinet.User) - 1);
    Cabinet.User[sizeof(Cabinet.User) - 1] = '\0';

    /* Notify ESP32 */
    {
        char evt[32];
        snprintf(evt, sizeof(evt), "%s:Unlock", g_identify_name);
        uart4_send_face_event(evt);
        printf("[FACE] Unlock event sent: %s\r\n", evt);
    }

    /* Start barcode scanner */
    Barcode_TaskHandle = osThreadNew(Barcode_Task, NULL, &BarcodeTask_attributes);

    /* Navigate to main screen */
    eez_flow_push_screen(SCREEN_ID_PAGE_MAIN, LV_SCR_LOAD_ANIM_NONE, 0, 0);
}