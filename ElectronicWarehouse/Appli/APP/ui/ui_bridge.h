#ifndef UI_BRIDGE_H
#define UI_BRIDGE_H

#include "app_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_set_integer(int var_id, int value);
int ui_get_integer(int var_id);
void ui_sync_cabinets(const InventoryItem *items, int count);

#ifdef __cplusplus
}
#endif

#endif
