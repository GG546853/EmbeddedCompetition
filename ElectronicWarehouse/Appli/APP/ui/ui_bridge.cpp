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
