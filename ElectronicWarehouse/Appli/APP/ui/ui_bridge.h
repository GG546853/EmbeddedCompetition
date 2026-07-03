#ifndef UI_BRIDGE_H
#define UI_BRIDGE_H

#include "app_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_set_integer(int var_id, int value);
int ui_get_integer(int var_id);
void ui_sync_cabinets(const InventoryItem *items, int count);
void ui_load_cabinets(InventoryItem *items, int count);
int  ui_get_log_count(void);
void ui_get_log_entry(int index, char *time, char *user, char *item,
                      int *qty, char *type, int *cab_id);

#ifdef __cplusplus
}
#endif

#endif
