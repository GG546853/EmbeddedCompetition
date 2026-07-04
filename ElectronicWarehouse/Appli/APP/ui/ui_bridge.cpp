#include "eez-flow.h"
#include "vars.h"
#include "structs.h"
#include "ui_bridge.h"
extern int history_count;
extern HistoryRecord history_list[32];

typedef void * osMutexId_t;
#define osWaitForever 0xFFFFFFFFU
extern "C" {
int32_t osMutexAcquire(osMutexId_t mutex_id, uint32_t timeout);
int32_t osMutexRelease(osMutexId_t mutex_id);
}
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

extern "C" void action_givetime(lv_event_t * e)
{
    char time_str[32];
    get_current_time_str(time_str, sizeof(time_str));

    Value records_val = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_HISTORY_RECORDS);
    if (!records_val.isArray()) return;

    ArrayOfLogEntryValue logs(records_val);
    int size = (int)logs.size();
    int count = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_HISTORY_COUNT).getInt();

    if (count < size) {
        LogEntryValue entry = logs.at(count);
        entry.time(time_str);
    }

    int new_count = count + 1;
    eez::flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_HISTORY_COUNT, eez::Value(new_count));

    if (new_count > history_count) {
        history_count = new_count;
    }
    ui_pull_history(history_list, new_count);
}
