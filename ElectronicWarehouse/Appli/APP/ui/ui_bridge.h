#ifndef UI_BRIDGE_H
#define UI_BRIDGE_H

#include "app_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_set_integer(int var_id, int value);
int ui_get_integer(int var_id);
void ui_set_string(int var_id, const char *value);
const char *ui_get_string(int var_id);
void ui_push_inventory(const InventoryItem *t_items, int t_count,
                       const InventoryItem *d_items, int d_count);
void ui_pull_inventory(InventoryItem *t_items, int t_count,
                       InventoryItem *d_items, int d_count);
void ui_push_history(const HistoryRecord *list, int count);
int  ui_pull_history(HistoryRecord *list, int max_count);
int  ui_get_log_count(void);
void ui_get_log_entry(int index, char *time, char *user, char *pc,
                      int *qty, char *action, int *cab_id);
void ui_push_alarm(const AlarmRecord *list, int count);

#ifdef __cplusplus
}
#endif

#endif
