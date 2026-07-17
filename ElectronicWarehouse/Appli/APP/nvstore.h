#ifndef __NVSTORE_H
#define __NVSTORE_H

#include <stdint.h>
#include "norflash.h"

/* ---- Storage Addresses in NOR Flash ---- */
#define NVSTORE_BASE                0x71E00000

#define NVSTORE_INV_SLOT_A_ADDR     (NVSTORE_BASE + 0x00000000)
#define NVSTORE_INV_SLOT_B_ADDR     (NVSTORE_BASE + 0x00002000)
#define NVSTORE_INV_SLOT_SIZE       0x2000  /* 8 KB = 2 sectors */

#define NVSTORE_FACE_SLOT_A_ADDR    (NVSTORE_BASE + 0x00004000)
#define NVSTORE_FACE_SLOT_B_ADDR    (NVSTORE_BASE + 0x00006000)
#define NVSTORE_FACE_SLOT_SIZE      0x2000  /* 8 KB = 2 sectors */

/* ---- Header Magic & Type Tags ---- */
#define NVSTORE_MAGIC               0x4E565453  /* "NVTS" */
#define NVSTORE_TYPE_INVENTORY      0x494E564E  /* "INVN" */
#define NVSTORE_TYPE_FACE           0x46414345  /* "FACE" */

/* ---- Status Codes ---- */
typedef enum {
    NVSTORE_OK             =  0,
    NVSTORE_ERROR_INIT     = -1,
    NVSTORE_ERROR_ERASE    = -2,
    NVSTORE_ERROR_WRITE    = -3,
    NVSTORE_ERROR_CRC      = -4,
    NVSTORE_ERROR_NODATA   = -5,
    NVSTORE_ERROR_PARAM    = -6,
} NVStore_Status;

/* ---- Public API ---- */

/** Initialize NVStore with an initialized NORFlash handle. */
NVStore_Status NVStore_Init(NORFlash_ObjectTypeDef *flashObj);

/** Save inventory arrays to NOR Flash (~830 ms, not ISR-safe). */
NVStore_Status NVStore_SaveInventory(void);

/** Load inventory from NOR Flash back to SRAM arrays. */
NVStore_Status NVStore_LoadInventory(void);

/** Save face gallery to NOR Flash (~830 ms, not ISR-safe). */
NVStore_Status NVStore_SaveFaceGallery(void);

/** Load face gallery from NOR Flash back to SRAM. */
NVStore_Status NVStore_LoadFaceGallery(void);

#endif /* __NVSTORE_H */
