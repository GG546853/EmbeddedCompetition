#ifndef __STORAGE_H
#define __STORAGE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief  Persistent storage module for inventory, face gallery, and history.
 *
 *  File format (binary, FATFS "0:" volume on SD NAND):
 *   - 0:invent_t.bin  → InventoryItem[28]
 *   - 0:invent_d.bin  → InventoryItem[6]
 *   - 0:faces.bin     → uint32_t count + face_entry_t[count]
 *   - 0:history.bin   → int32_t  count + HistoryRecord[count]
 *
 *  Each file is fully rewritten on save.
 *  First boot (file missing) is silent — arrays stay zeroed.
 */

/* ---- API ---- */

int  storage_init(void);

int  storage_load_inventory(void);
int  storage_save_inventory(void);

int  storage_load_faces(void);
int  storage_save_faces(void);

int  storage_load_history(void);
int  storage_save_history(void);

/* ---- Deferred flush (called from low-priority task) ---- */

void storage_mark_inventory_dirty(void);
void storage_mark_faces_dirty(void);
void storage_mark_history_dirty(void);
void storage_flush(void);              /* write all dirty data now */

/* ---- Mutex (created in app_freertos.c) ---- */
extern void *storage_mutex;           /* osMutexId_t, used by flush task */

#endif
