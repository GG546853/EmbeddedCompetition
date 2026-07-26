/**
 ****************************************************************************************************
 * @file        storage.c
 * @brief       Persistent storage module — FATFS on SD NAND
 *
 *  Saves/loads inventory arrays, face gallery, and history records
 *  using binary files. Deferred flush via dirty flags to avoid
 *  blocking UI/AI tasks on SD write latency.
 ****************************************************************************************************
 */

#include "storage.h"
#include "ff.h"
#include "cmsis_os2.h"
#include "app_types.h"
#include "app_x-cube-ai.h"
#include "Outbound_task.h"
#include <stdio.h>
#include <string.h>

/* ---- FATFS object (single volume) ---- */
static FATFS g_fs;

/* ---- Dirty flags ---- */
static volatile bool g_inventory_dirty = false;
static volatile bool g_faces_dirty     = false;
static volatile bool g_history_dirty   = false;

/* ---- Mutex (set by app_freertos.c) ---- */
void *storage_mutex = NULL;

/* ---- Forward declarations ---- */
static int save_buf(const char *path, const void *buf, uint32_t size);
static int load_buf(const char *path, void *buf, uint32_t size, uint32_t *out_read);

/* ========================================================================== */
/*  Init / Mount                                                              */
/* ========================================================================== */

int storage_init(void)
{
    FRESULT res;
    BYTE work[FF_MAX_SS];

    res = f_mount(&g_fs, "0:", 1);  /* 1 = force immediate mount */
    if (res == FR_NO_FILESYSTEM) {
        printf("[STORAGE] No filesystem, formatting...\r\n");
        res = f_mkfs("0:", 0, work, sizeof(work));
        if (res != FR_OK) {
            printf("[STORAGE] Format failed (%d)\r\n", res);
            return -1;
        }
        printf("[STORAGE] Format OK\r\n");
        res = f_mount(&g_fs, "0:", 1);
    }
    if (res != FR_OK) {
        printf("[STORAGE] Mount failed (%d)\r\n", res);
        return -1;
    }
    printf("[STORAGE] Mount OK\r\n");
    return 0;
}

/* ========================================================================== */
/*  Inventory (28 × T-type + 6 × D-type)                                      */
/* ========================================================================== */

int storage_load_inventory(void)
{
    uint32_t br;

    if (load_buf("0:invent_t.bin", inventory_item_T, sizeof(inventory_item_T), &br) == 0) {
        printf("[STORAGE] Loaded invent_t.bin (%lu / %u)\r\n",
               (unsigned long)br, (unsigned int)sizeof(inventory_item_T));
    }
    // else: first boot — arrays are already zeroed

    if (load_buf("0:invent_d.bin", inventory_item_D, sizeof(inventory_item_D), &br) == 0) {
        printf("[STORAGE] Loaded invent_d.bin (%lu / %u)\r\n",
               (unsigned long)br, (unsigned int)sizeof(inventory_item_D));
    }

    return 0;
}

int storage_save_inventory(void)
{
    int ret = 0;

    if (save_buf("0:invent_t.bin", inventory_item_T, sizeof(inventory_item_T)) != 0) ret = -1;
    if (save_buf("0:invent_d.bin", inventory_item_D, sizeof(inventory_item_D)) != 0) ret = -1;

    if (ret == 0) {
        printf("[STORAGE] Inventory saved\r\n");
    }
    return ret;
}

/* ========================================================================== */
/*  Face Gallery                                                               */
/* ========================================================================== */

int storage_load_faces(void)
{
	static face_entry_t entries[FACE_GALLERY_MAX];
    uint32_t     count = 0;
    FIL file;
    UINT br;

    if (f_open(&file, "0:faces.bin", FA_READ) != FR_OK) {
        printf("[STORAGE] No faces.bin (first boot)\r\n");
        return -1;
    }

    f_read(&file, &count, sizeof(count), &br);
    if (br != sizeof(count) || count > FACE_GALLERY_MAX) {
        printf("[STORAGE] faces.bin header invalid (count=%lu)\r\n", (unsigned long)count);
        f_close(&file);
        return -1;
    }

    f_read(&file, entries, count * sizeof(face_entry_t), &br);
    f_close(&file);

    ai_face_gallery_import(entries, count);
    printf("[STORAGE] Face gallery loaded (%lu faces)\r\n", (unsigned long)count);
    return 0;
}

int storage_save_faces(void)
{
	static face_entry_t entries[FACE_GALLERY_MAX];
    uint32_t     count = 0;
    FIL file;
    UINT bw;

    ai_face_gallery_export(entries, &count);

    FRESULT res = f_open(&file, "0:faces.bin", FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        printf("[STORAGE] faces.bin open failed (FRESULT=%d)\r\n", res);
        return -1;
    }

    if (f_write(&file, &count, sizeof(count), &bw) != FR_OK || bw != sizeof(count)) {
        printf("[STORAGE] faces.bin header write failed\r\n");
        f_close(&file);
        return -1;
    }

    if (count > 0) {
        f_write(&file, entries, count * sizeof(face_entry_t), &bw);
    }
    f_close(&file);

    printf("[STORAGE] Face gallery saved (%lu faces)\r\n", (unsigned long)count);
    return 0;
}

/* ========================================================================== */
/*  History                                                                    */
/* ========================================================================== */

extern HistoryRecord history_list[32];
extern int history_count;

int storage_load_history(void)
{
    return 0; /* disabled: history SD persistence removed */
#if 0
    FIL file;
    UINT br;
    int32_t count = 0;

    if (f_open(&file, "0:history.bin", FA_READ) != FR_OK) {
        printf("[STORAGE] No history.bin (first boot)\r\n");
        return -1;
    }

    f_read(&file, &count, sizeof(count), &br);
    if (br != sizeof(count) || count < 0 || count > 32) {
        printf("[STORAGE] history.bin header invalid (count=%ld)\r\n", (long)count);
        f_close(&file);
        return -1;
    }

    if (count > 0) {
        f_read(&file, history_list, count * sizeof(HistoryRecord), &br);
    }
    f_close(&file);

    history_count = (int)count;
    printf("[STORAGE] History loaded (%d records)\r\n", history_count);
    return 0;
#endif
}

int storage_save_history(void)
{
    return 0; /* disabled: history SD persistence removed */
#if 0
    FIL file;
    UINT bw;
    int32_t count = (int32_t)history_count;

    if (f_open(&file, "0:history.bin", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
        printf("[STORAGE] history.bin open failed\r\n");
        return -1;
    }

    if (f_write(&file, &count, sizeof(count), &bw) != FR_OK || bw != sizeof(count)) {
        f_close(&file);
        return -1;
    }

    if (count > 0) {
        f_write(&file, history_list, count * sizeof(HistoryRecord), &bw);
    }
    f_close(&file);

    printf("[STORAGE] History saved (%d records)\r\n", history_count);
    return 0;
#endif
}

/* ========================================================================== */
/*  Dirty flags & flush                                                       */
/* ========================================================================== */

void storage_mark_inventory_dirty(void) { g_inventory_dirty = true; }
void storage_mark_faces_dirty(void)     { g_faces_dirty     = true; }
// void storage_mark_history_dirty(void)   { g_history_dirty   = true; }
void storage_mark_history_dirty(void)   { /* disabled */ }

void storage_flush(void)
{
    if (!g_inventory_dirty && !g_faces_dirty && !g_history_dirty)
        return;

    if (storage_mutex != NULL) {
        osMutexAcquire(storage_mutex, osWaitForever);
    }

    if (g_inventory_dirty) {
        if (storage_save_inventory() == 0)
            g_inventory_dirty = false;
    }
    if (g_faces_dirty) {
        if (storage_save_faces() == 0)
            g_faces_dirty = false;
    }
    // if (g_history_dirty) {
    //     if (storage_save_history() == 0)
    //         g_history_dirty = false;
    // }

    if (storage_mutex != NULL) {
        osMutexRelease(storage_mutex);
    }
}

/* ========================================================================== */
/*  Helpers — raw binary save / load                                          */
/* ========================================================================== */

static int save_buf(const char *path, const void *buf, uint32_t size)
{
    FIL file;
    UINT bw;

    FRESULT res = f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        printf("[STORAGE] %s open failed (FRESULT=%d)\r\n", path, (int)res);
        return -1;
    }

    res = f_write(&file, buf, size, &bw);
    f_close(&file);

    if (res != FR_OK || bw != size) {
        printf("[STORAGE] %s write failed (%d, %u/%lu)\r\n", path, res,
               (unsigned int)bw, (unsigned long)size);
        return -1;
    }

    return 0;
}

static int load_buf(const char *path, void *buf, uint32_t size, uint32_t *out_read)
{
    FIL file;
    UINT br;

    if (f_open(&file, path, FA_READ) != FR_OK) {
        return -1;  // silent — first boot
    }

    f_read(&file, buf, size, &br);
    f_close(&file);

    if (out_read) *out_read = br;
    return 0;
}
