#include "nvstore.h"
#include "app_types.h"
#include "app_x-cube-ai.h"
#include "Outbound_task.h"
#include <string.h>

/* ---- Slot Header (packed, 20 bytes) ---- */
typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t type;
    uint32_t version;
    uint32_t payload_size;
    uint32_t crc32;
} SlotHeader;

/* ---- Module state ---- */
static NORFlash_ObjectTypeDef *nv_flash;

/* ---- CRC32 (Ethernet polynomial) ---- */
static uint32_t crc32(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFF;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return ~crc;
}

/* ---- Read header via memory-mapped XIP ---- */
static void read_header(uint32_t slot_addr, SlotHeader *hdr)
{
    memcpy(hdr, (const void *)slot_addr, sizeof(SlotHeader));
}

/* ---- Validate a slot: magic + type + payload bounds + CRC ---- */
static bool slot_is_valid(uint32_t slot_addr, uint32_t slot_size,
                          uint32_t expected_type, uint32_t *out_version)
{
    SlotHeader hdr;
    read_header(slot_addr, &hdr);

    if (hdr.magic != NVSTORE_MAGIC)         return false;
    if (hdr.type  != expected_type)         return false;
    if (hdr.payload_size == 0)              return false;
    if (hdr.payload_size > (slot_size - sizeof(SlotHeader))) return false;

    const uint8_t *payload = (const uint8_t *)(slot_addr + sizeof(SlotHeader));
    uint32_t computed = crc32(payload, hdr.payload_size);
    if (computed != hdr.crc32)              return false;

    if (out_version) *out_version = hdr.version;
    return true;
}

/* ---- Find the valid slot with the highest version ---- */
static uint32_t find_best_slot(uint32_t addr_a, uint32_t addr_b, uint32_t slot_size,
                               uint32_t expected_type, uint32_t *out_version)
{
    uint32_t ver_a = 0, ver_b = 0;
    bool ok_a = slot_is_valid(addr_a, slot_size, expected_type, &ver_a);
    bool ok_b = slot_is_valid(addr_b, slot_size, expected_type, &ver_b);

    if (ok_a && ok_b) {
        *out_version = (ver_a >= ver_b) ? ver_a : ver_b;
        return (ver_a >= ver_b) ? addr_a : addr_b;
    }
    if (ok_a) { *out_version = ver_a; return addr_a; }
    if (ok_b) { *out_version = ver_b; return addr_b; }

    *out_version = 0;
    return 0;
}

/* ---- Pick the slot to overwrite (lower version or invalid) ---- */
static uint32_t find_target_slot(uint32_t addr_a, uint32_t addr_b, uint32_t slot_size,
                                 uint32_t expected_type)
{
    uint32_t ver_a = 0, ver_b = 0;
    bool ok_a = slot_is_valid(addr_a, slot_size, expected_type, &ver_a);
    bool ok_b = slot_is_valid(addr_b, slot_size, expected_type, &ver_b);

    if (!ok_a && !ok_b) return addr_a;       /* start with slot A */
    if (!ok_a) return addr_a;                /* overwrite the invalid one */
    if (!ok_b) return addr_b;
    return (ver_a <= ver_b) ? addr_a : addr_b; /* overwrite the older one */
}

/**
 * Critical erase+write routine — must execute from SRAM (.RamFunc) so that
 * the CPU never tries to fetch instructions from NOR Flash while the XSPI
 * bus is occupied with the erase / program operation.
 */
__attribute__((section(".RamFunc")))
static NVStore_Status erase_and_write(uint32_t addr, uint32_t erase_size,
                                      const uint8_t *data, uint32_t data_len)
{
    NVStore_Status status = NVSTORE_OK;

    __disable_irq();

    if (NORFlash_EraseSector(nv_flash, addr, erase_size) != NORFlash_OK) {
        status = NVSTORE_ERROR_ERASE;
        goto exit;
    }

    if (NORFlash_Write(nv_flash, addr, data, data_len) != NORFlash_OK) {
        status = NVSTORE_ERROR_WRITE;
    }

exit:
    __enable_irq();
    return status;
}

/* ==================================================================
 *  Public API
 * ================================================================== */

NVStore_Status NVStore_Init(NORFlash_ObjectTypeDef *flashObj)
{
    if (!flashObj) return NVSTORE_ERROR_PARAM;
    nv_flash = flashObj;
    return NVSTORE_OK;
}

/* ---- Inventory --------------------------------------------------- */

NVStore_Status NVStore_LoadInventory(void)
{
    if (!nv_flash) return NVSTORE_ERROR_INIT;

    uint32_t version;
    uint32_t best = find_best_slot(NVSTORE_INV_SLOT_A_ADDR, NVSTORE_INV_SLOT_B_ADDR,
                                   NVSTORE_INV_SLOT_SIZE, NVSTORE_TYPE_INVENTORY, &version);
    if (!best) return NVSTORE_ERROR_NODATA;

    const uint8_t *p = (const uint8_t *)(best + sizeof(SlotHeader));

    /* skip count_T / count_D — arrays are fixed size */
    p += 8;
    memcpy(inventory_item_T, p, sizeof(inventory_item_T));    p += sizeof(inventory_item_T);
    memcpy(inventory_item_D, p, sizeof(inventory_item_D));

    return NVSTORE_OK;
}

NVStore_Status NVStore_SaveInventory(void)
{
    if (!nv_flash) return NVSTORE_ERROR_INIT;

    const uint32_t payload_size = 8
        + sizeof(inventory_item_T)
        + sizeof(inventory_item_D);

    /* Find current version and choose target slot */
    uint32_t version;
    find_best_slot(NVSTORE_INV_SLOT_A_ADDR, NVSTORE_INV_SLOT_B_ADDR,
                   NVSTORE_INV_SLOT_SIZE, NVSTORE_TYPE_INVENTORY, &version);

    uint32_t target = find_target_slot(NVSTORE_INV_SLOT_A_ADDR, NVSTORE_INV_SLOT_B_ADDR,
                                       NVSTORE_INV_SLOT_SIZE, NVSTORE_TYPE_INVENTORY);

    /* Build header + payload in RAM buffer */
    static uint8_t buf[NVSTORE_INV_SLOT_SIZE] __attribute__((aligned(32)));

    SlotHeader *hdr = (SlotHeader *)buf;
    hdr->magic        = NVSTORE_MAGIC;
    hdr->type         = NVSTORE_TYPE_INVENTORY;
    hdr->version      = version + 1;
    hdr->payload_size = payload_size;

    uint32_t cnt = 28; memcpy(buf + sizeof(SlotHeader) + 0, &cnt, 4);
    cnt = 6;          memcpy(buf + sizeof(SlotHeader) + 4, &cnt, 4);

    uint8_t *p = buf + sizeof(SlotHeader) + 8;
    memcpy(p, inventory_item_T, sizeof(inventory_item_T));   p += sizeof(inventory_item_T);
    memcpy(p, inventory_item_D, sizeof(inventory_item_D));

    hdr->crc32 = crc32(buf + sizeof(SlotHeader), payload_size);

    return erase_and_write(target, NVSTORE_INV_SLOT_SIZE,
                           buf, sizeof(SlotHeader) + payload_size);
}

/* ---- Face Gallery ------------------------------------------------ */

NVStore_Status NVStore_LoadFaceGallery(void)
{
    if (!nv_flash) return NVSTORE_ERROR_INIT;

    uint32_t version;
    uint32_t best = find_best_slot(NVSTORE_FACE_SLOT_A_ADDR, NVSTORE_FACE_SLOT_B_ADDR,
                                   NVSTORE_FACE_SLOT_SIZE, NVSTORE_TYPE_FACE, &version);
    if (!best) return NVSTORE_ERROR_NODATA;

    const uint8_t *p = (const uint8_t *)(best + sizeof(SlotHeader));

    uint32_t count;
    memcpy(&count, p, 4);  p += 4;

    static face_entry_t entries[FACE_GALLERY_MAX];
    memcpy(entries, p, sizeof(entries));

    ai_face_gallery_import(entries, count);
    return NVSTORE_OK;
}

NVStore_Status NVStore_SaveFaceGallery(void)
{
    if (!nv_flash) return NVSTORE_ERROR_INIT;

    const uint32_t payload_size = 4 + sizeof(face_entry_t) * FACE_GALLERY_MAX;

    static face_entry_t entries[FACE_GALLERY_MAX];
    uint32_t count;
    ai_face_gallery_export(entries, &count);

    uint32_t version;
    find_best_slot(NVSTORE_FACE_SLOT_A_ADDR, NVSTORE_FACE_SLOT_B_ADDR,
                   NVSTORE_FACE_SLOT_SIZE, NVSTORE_TYPE_FACE, &version);

    uint32_t target = find_target_slot(NVSTORE_FACE_SLOT_A_ADDR, NVSTORE_FACE_SLOT_B_ADDR,
                                       NVSTORE_FACE_SLOT_SIZE, NVSTORE_TYPE_FACE);

    static uint8_t buf[NVSTORE_FACE_SLOT_SIZE] __attribute__((aligned(32)));

    SlotHeader *hdr = (SlotHeader *)buf;
    hdr->magic        = NVSTORE_MAGIC;
    hdr->type         = NVSTORE_TYPE_FACE;
    hdr->version      = version + 1;
    hdr->payload_size = payload_size;

    memcpy(buf + sizeof(SlotHeader), &count, 4);
    memcpy(buf + sizeof(SlotHeader) + 4, entries, sizeof(entries));

    hdr->crc32 = crc32(buf + sizeof(SlotHeader), payload_size);

    return erase_and_write(target, NVSTORE_FACE_SLOT_SIZE,
                           buf, sizeof(SlotHeader) + payload_size);
}
