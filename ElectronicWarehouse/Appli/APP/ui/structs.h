#ifndef EEZ_LVGL_UI_STRUCTS_H
#define EEZ_LVGL_UI_STRUCTS_H

#include "eez-flow.h"

#include <stdint.h>
#include <stdbool.h>

#include "vars.h"

using namespace eez;

enum FlowStructures {
    FLOW_STRUCTURE_CABINET = 16384,
    FLOW_STRUCTURE_LOG_ENTRY = 16385
};

enum FlowArrayOfStructures {
    FLOW_ARRAY_OF_STRUCTURE_CABINET = 81920,
    FLOW_ARRAY_OF_STRUCTURE_LOG_ENTRY = 81921
};

enum CabinetFlowStructureFields {
    FLOW_STRUCTURE_CABINET_FIELD_NAME = 0,
    FLOW_STRUCTURE_CABINET_FIELD_QTY = 1,
    FLOW_STRUCTURE_CABINET_FIELD_TYPE = 2,
    FLOW_STRUCTURE_CABINET_FIELD_SPEC = 3,
    FLOW_STRUCTURE_CABINET_NUM_FIELDS
};

enum LogEntryFlowStructureFields {
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_TIME = 0,
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_USER = 1,
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_ITEM_NAME = 2,
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_QTY = 3,
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_TYPE = 4,
    FLOW_STRUCTURE_LOG_ENTRY_FIELD_CABINET_ID = 5,
    FLOW_STRUCTURE_LOG_ENTRY_NUM_FIELDS
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
    
    int qty() {
        return value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_QTY].getInt();
    }
    void qty(int qty) {
        value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_QTY] = IntegerValue(qty);
    }
    
    const char *type() {
        return value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_TYPE].getString();
    }
    void type(const char *type) {
        value.getArray()->values[FLOW_STRUCTURE_CABINET_FIELD_TYPE] = StringValue(type);
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
    
    const char *item_name() {
        return value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_ITEM_NAME].getString();
    }
    void item_name(const char *item_name) {
        value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_ITEM_NAME] = StringValue(item_name);
    }
    
    int qty() {
        return value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_QTY].getInt();
    }
    void qty(int qty) {
        value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_QTY] = IntegerValue(qty);
    }
    
    const char *type() {
        return value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_TYPE].getString();
    }
    void type(const char *type) {
        value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_TYPE] = StringValue(type);
    }
    
    int cabinet_id() {
        return value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_CABINET_ID].getInt();
    }
    void cabinet_id(int cabinet_id) {
        value.getArray()->values[FLOW_STRUCTURE_LOG_ENTRY_FIELD_CABINET_ID] = IntegerValue(cabinet_id);
    }
};

typedef ArrayOf<LogEntryValue, FLOW_ARRAY_OF_STRUCTURE_LOG_ENTRY> ArrayOfLogEntryValue;

#endif /*EEZ_LVGL_UI_STRUCTS_H*/