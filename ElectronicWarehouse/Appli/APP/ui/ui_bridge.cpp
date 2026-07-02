#include "eez-flow.h"
#include "vars.h"
#include "structs.h"
#include "ui_bridge.h"

void ui_set_integer(int var_id, int value) {
    eez::flow::setGlobalVariable(var_id, eez::Value(value));
}

int ui_get_integer(int var_id) {
    return eez::flow::getGlobalVariable(var_id).getInt();
}

void ui_sync_cabinets(const InventoryItem *items, int count) {
    ArrayOfCabinetValue cabinets(count);
    for (int i = 0; i < count; i++) {
        CabinetValue cab;
        cab.name(items[i].name);
        cab.qty(items[i].quantity);
        cab.pa(items[i].pa);
        cab.spec(items[i].type);
        cab.pc(items[i].pc);
        cabinets.at(i, cab);
    }
    eez::flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_CABINETS, cabinets);
    ui_set_integer(FLOW_GLOBAL_VARIABLE_CABINET_COUNT, count);
}

int ui_get_log_count(void) {
    Value v = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_HISTORY_RECORDS);
    if (!v.isArray()) return 0;
    return (int)v.getArray()->arraySize;
}

void ui_get_log_entry(int index, char *time, char *user, char *item,
                      int *qty, char *type, int *cab_id) {
    Value v = eez::flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_HISTORY_RECORDS);
    if (!v.isArray()) return;
    ArrayOfLogEntryValue logs(v);
    if (index >= (int)logs.size()) return;
    LogEntryValue entry = logs.at(index);
    strcpy(time, entry.time());
    strcpy(user, entry.user());
    strcpy(item, entry.item_name());
    *qty   = entry.qty();
    strcpy(type, entry.type());
    *cab_id = entry.cabinet_id();
}
