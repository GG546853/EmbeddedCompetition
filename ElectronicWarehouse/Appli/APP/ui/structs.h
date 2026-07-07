#ifndef EEZ_LVGL_UI_STRUCTS_H
#define EEZ_LVGL_UI_STRUCTS_H

#include "eez-flow.h"

#include <stdint.h>
#include <stdbool.h>

#include "vars.h"

using namespace eez;

enum FlowStructures {
    FLOW_STRUCTURE_CABINET = 16384,
    FLOW_STRUCTURE_LOG_ENTRY = 16385,
    FLOW_STRUCTURE_LSW = 16386
};

enum FlowArrayOfStructures {
    FLOW_ARRAY_OF_STRUCTURE_CABINET = 81920,
    FLOW_ARRAY_OF_STRUCTURE_LOG_ENTRY = 81921,
    FLOW_ARRAY_OF_STRUCTURE_LSW = 81922
};

enum CabinetFlowStructureFields {
    FLOW_STRUCTURE_CABINET_FIELD_NAME = 0,
    FLOW_STRUCTURE_CABINET_FIELD_PC = 1,
    FLOW_STRUCTURE_CABINET_FIELD_QTY = 2,
    FLOW_STRUCTURE_CABINET_FIELD_PA = 3,
    FLOW_STRUCTURE_CABINET_FIELD_SPEC = 4,
    FLOW_STRUCTURE_CABINET_NUM_FIELDS
};

enum LogEntryFlowStructureFields {
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_TIME = 0,
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_USER = 1,
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_PC = 2,
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_QTY = 3,
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_ACTION = 4,
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_CABINET_ID = 5,
    FLOW_STRUCTURE_LOG_ENTRY_NUM_FIELDS
};

enum LSWFlowStructureFields {
    FLOW_STRUCTURE_LSW_FIELD_NAME = 0,
    FLOW_STRUCTURE_LSW_FIELD_CNUM = 1,
    FLOW_STRUCTURE_LSW_FIELD_STOCK = 2,
    FLOW_STRUCTURE_LSW_NUM_FIELDS
};

struct CabinetValue {
    Value value;
    
    CabinetValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_CABINET_NUM_FIELDS, FLOW_STRUCTURE_CABINET, 0);
    }
    
    CabinetValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *name() {
        return value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_NAME].getString();
    }
    void name(const char *name) {
        value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_NAME] = StringValue(name);
    }
    
    const char *pc() {
        return value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_PC].getString();
    }
    void pc(const char *pc) {
        value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_PC] = StringValue(pc);
    }
    
    int qty() {
        return value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_QTY].getInt();
    }
    void qty(int qty) {
        value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_QTY] = IntegerValue(qty);
    }
    
    const char *pa() {
        return value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_PA].getString();
    }
    void pa(const char *pa) {
        value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_PA] = StringValue(pa);
    }
    
    const char *spec() {
        return value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_SPEC].getString();
    }
    void spec(const char *spec) {
        value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_SPEC] = StringValue(spec);
    }
};

typedef ArrayOf<CabinetValue, FLOW_ARRAY_OF_STRUCTURE_CABINET> ArrayOfCabinetValue;
struct LogEntryValue {
    Value value;
    
    LogEntryValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_LOG_ENTRY_NUM_FIELDS, FLOW_STRUCTURE_LOG_ENTRY, 0);
    }
    
    LogEntryValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *time() {
        return value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_TIME].getString();
    }
    void time(const char *time) {
        value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_TIME] = StringValue(time);
    }
    
    const char *user() {
        return value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_USER].getString();
    }
    void user(const char *user) {
        value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_USER] = StringValue(user);
    }
    
    const char *pc() {
        return value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_PC].getString();
    }
    void pc(const char *pc) {
        value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_PC] = StringValue(pc);
    }
    
    int qty() {
        return value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_QTY].getInt();
    }
    void qty(int qty) {
        value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_QTY] = IntegerValue(qty);
    }
    
    const char *action() {
        return value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_ACTION].getString();
    }
    void action(const char *action) {
        value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_ACTION] = StringValue(action);
    }
    
    int cabinet_id() {
        return value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_CABINET_ID].getInt();
    }
    void cabinet_id(int cabinet_id) {
        value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_CABINET_ID] = IntegerValue(cabinet_id);
    }
};

typedef ArrayOf<LogEntryValue, FLOW_ARRAY_OF_STRUCTURE_LOG_ENTRY> ArrayOfLogEntryValue;
struct LSWValue {
    Value value;
    
    LSWValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_LSW_NUM_FIELDS, FLOW_STRUCTURE_LSW, 0);
    }
    
    LSWValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *name() {
        return value.getArray()->values[FLOW_STRUCTURE_LSW_FIELD_NAME].getString();
    }
    void name(const char *name) {
        value.getArray()->values[FLOW_STRUCTURE_LSW_FIELD_NAME] = StringValue(name);
    }
    
    int CNUM() {
        return value.getArray()->values[FLOW_STRUCTURE_LSW_FIELD_CNUM].getInt();
    }
    void CNUM(int CNUM) {
        value.getArray()->values[FLOW_STRUCTURE_LSW_FIELD_CNUM] = IntegerValue(CNUM);
    }
    
    int STOCK() {
        return value.getArray()->values[FLOW_STRUCTURE_LSW_FIELD_STOCK].getInt();
    }
    void STOCK(int STOCK) {
        value.getArray()->values[FLOW_STRUCTURE_LSW_FIELD_STOCK] = IntegerValue(STOCK);
    }
};

typedef ArrayOf<LSWValue, FLOW_ARRAY_OF_STRUCTURE_LSW> ArrayOfLSWValue;

#endif /*EEZ_LVGL_UI_STRUCTS_H*/