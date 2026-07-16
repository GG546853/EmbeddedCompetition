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

/* ==================================================================
 *  XIP-safe register-level helpers (all .RamFunc — zero Flash calls)
 * ================================================================== */

/* 8D8D8D instruction encoding: CMD<<8 | ~CMD */
#define XIP_INSTR(cmd)  ((uint32_t)(((uint16_t)(cmd) << 8) | ((uint8_t)(~(cmd)) & 0xFF)))

/* CCR values for 8D-8D-8D DTR mode */
#define XIP_CCR_INSTR         0x0000003F  /* Instruction only */
#define XIP_CCR_INSTR_ADDR    0x00003F3F  /* Instruction + Address */
#define XIP_CCR_INSTR_ADDR_D  0x0F003F3F  /* + Data (no DQS, for writes) */
#define XIP_CCR_INSTR_ADDR_DQ 0x2F003F3F  /* + Data + DQS (for reads) */

/* Timeout loop counts (~6 cycles/iter at 800 MHz → ~7.5 ns/iter) */
#define XIP_TO_SHORT     2000000   /* ~15 ms */
#define XIP_TO_ERASE   800000000   /* ~6 s (covers max 5 s sector erase) */
#define XIP_TO_PROG     20000000   /* ~150 ms (covers max 5 ms page program) */

/* ---- Wait for flash ready via auto-polling RDSR (WIP=0) ---- */
__attribute__((section(".RamFunc")))
static int xip_wait_ready(XSPI_TypeDef *xspi, volatile uint32_t timeout)
{
    uint32_t cr;

    /* Step 1: Set FMODE=0 first (indirect write) before touching any config regs */
    cr  = xspi->CR;
    cr &= ~(XSPI_CR_FMODE | XSPI_CR_PMM | XSPI_CR_APMS);
    cr |= XSPI_CR_EN;
    xspi->CR = cr;

    /* Step 2: Write config registers while FMODE=0.
     * Do NOT write AR here — in FMODE=0 it would trigger an indirect transfer. */
    xspi->CCR   = XIP_CCR_INSTR_ADDR_DQ;
    xspi->TCR   = 4;                    /* DCYC=4 */
    xspi->IR    = XIP_INSTR(0x05);      /* RDSR */
    xspi->DLR   = 0;                    /* 1 byte */
    xspi->PSMKR = 0x01;                 /* mask WIP (bit 0) */
    xspi->PSMAR = 0x00;                 /* match WIP=0 */
    xspi->PIR   = 0x10;                 /* polling interval */

    /* Step 3: Switch to auto-polling mode (FMODE=2) */
    cr  = xspi->CR;
    cr &= ~(XSPI_CR_FMODE | XSPI_CR_PMM | XSPI_CR_APMS);
    cr |= (2U << XSPI_CR_FMODE_Pos) | XSPI_CR_APMS;  /* FMODE=2, APMS, AND-match */
    xspi->CR = cr | XSPI_CR_EN;

    /* Step 4: Trigger auto-polling by writing AR (now in FMODE=2) */
    xspi->AR = 0;

    while (!(xspi->SR & XSPI_SR_SMF) && --timeout > 0) {}
    xspi->FCR = XSPI_FCR_CSMF;

    return (timeout > 0) ? 0 : -1;
}

/* ---- Send instruction-only command (e.g. WREN 0x06) ---- */
__attribute__((section(".RamFunc")))
static int xip_send_cmd_instr(XSPI_TypeDef *xspi, uint8_t cmd, volatile uint32_t timeout)
{
    uint32_t cr;

    /* Step 1: Set FMODE=0 first (indirect write) — previous op may have left FMODE=2 */
    cr  = xspi->CR;
    cr &= ~(XSPI_CR_FMODE | XSPI_CR_APMS | XSPI_CR_PMM);
    cr |= XSPI_CR_EN;
    xspi->CR = cr;

    /* Step 2: Write CCR/TCR/IR — IR write triggers the transfer in FMODE=0 */
    xspi->CCR = XIP_CCR_INSTR;
    xspi->TCR = 0;
    xspi->IR  = XIP_INSTR(cmd);

    /* Step 3: Wait for BUSY to de-assert, then for TCF */
    while ((xspi->SR & XSPI_SR_BUSY) && --timeout > 0) {}
    while (!(xspi->SR & XSPI_SR_TCF) && --timeout > 0) {}
    xspi->FCR = XSPI_FCR_CTCF;

    return (timeout > 0) ? 0 : -1;
}

/* ---- Send instruction+address command (e.g. Sector Erase 0x21) ---- */
__attribute__((section(".RamFunc")))
static int xip_send_cmd_addr(XSPI_TypeDef *xspi, uint8_t cmd, uint32_t addr,
                             volatile uint32_t timeout)
{
    uint32_t cr;

    /* Step 1: Set FMODE=0 first (indirect write) — previous op may have left FMODE=2 */
    cr  = xspi->CR;
    cr &= ~(XSPI_CR_FMODE | XSPI_CR_APMS | XSPI_CR_PMM);
    cr |= XSPI_CR_EN;
    xspi->CR = cr;

    /* Step 2: Write CCR/TCR/IR/AR — AR write triggers the transfer in FMODE=0 */
    xspi->CCR = XIP_CCR_INSTR_ADDR;
    xspi->TCR = 0;
    xspi->IR  = XIP_INSTR(cmd);
    xspi->AR  = addr;

    /* Step 3: Wait for BUSY to de-assert, then for TCF */
    while ((xspi->SR & XSPI_SR_BUSY) && --timeout > 0) {}
    while (!(xspi->SR & XSPI_SR_TCF) && --timeout > 0) {}
    xspi->FCR = XSPI_FCR_CTCF;

    return (timeout > 0) ? 0 : -1;
}

/**
 * Critical erase+write routine — must execute from SRAM (.RamFunc).
 *
 * LRUN:  code runs from SRAM; HAL/NORFlash calls are safe.
 * XIP:   code runs from NOR Flash; ALL operations during flash busy
 *        use direct XSPI2 register writes from within .RamFunc so
 *        the CPU never fetches instructions from Flash.
 */
__attribute__((section(".RamFunc")))
static NVStore_Status erase_and_write(uint32_t addr, uint32_t erase_size,
                                      const uint8_t *data, uint32_t data_len)
{
    int lrun = ((SCB->VTOR & 0xFF000000) == 0x34000000);

    __disable_irq();

    /* ---- LRUN path: HAL functions are safe (code in SRAM) ---- */
    if (lrun) {
        NVStore_Status status = NVSTORE_OK;
        NORFlash_DisableMemoryMappedMode(nv_flash);
        if (NORFlash_EraseSector(nv_flash, addr, erase_size) != NORFlash_OK) {
            status = NVSTORE_ERROR_ERASE;
            goto lrun_exit;
        }
        if (NORFlash_Write(nv_flash, addr, data, data_len) != NORFlash_OK) {
            status = NVSTORE_ERROR_WRITE;
        }
    lrun_exit:
        NORFlash_EnableMemoryMappedMode(nv_flash);
        __enable_irq();
        return status;
    }

    /* ---- XIP path: direct XSPI2 register writes, zero Flash calls ---- */
    {
        XSPI_TypeDef *const xspi = XSPI2;
        const uint32_t sector_sz = 0x1000;  /* 4 KB */
        const uint32_t page_sz   = 0x100;   /* 256 B */
        uint32_t cur_addr, remaining, chunk, cr;
        const uint8_t *src;

        /* Save MM-mode config so we can restore it after */
        uint32_t save_ccr  = xspi->CCR;
        uint32_t save_tcr  = xspi->TCR;
        uint32_t save_ir   = xspi->IR;
        uint32_t save_wccr = xspi->WCCR;
        uint32_t save_wtcr = xspi->WTCR;
        uint32_t save_wir  = xspi->WIR;

        /* ---- Exit memory-mapped mode ---- */
        /* Disable I-Cache to prevent speculative instruction fetches from NOR Flash
         * while the XSPI is in indirect mode (Port2 inactive). */
        SCB->CCR &= ~SCB_CCR_IC_Msk;
        __DSB();
        __ISB();
        SCB->ICIALLU = 0UL;
        __DSB();
        __ISB();

        /* Abort any in-flight MM transaction, then switch to indirect write mode.
         * Matches HAL_XSPI_Abort — just ABORT → clear FMODE, no EN toggle. */
        if (xspi->SR & XSPI_SR_BUSY) {
            cr  = xspi->CR;
            cr |= XSPI_CR_ABORT;
            xspi->CR = cr;
            { volatile uint32_t _t = XIP_TO_SHORT;
              while ((xspi->SR & XSPI_SR_BUSY) && --_t > 0) {} }
            { volatile uint32_t _t = XIP_TO_SHORT;
              while (!(xspi->SR & XSPI_SR_TCF) && --_t > 0) {} }
            xspi->FCR = XSPI_FCR_CTCF;
        }
        /* Clear FMODE to enter indirect write mode */
        cr  = xspi->CR;
        cr &= ~XSPI_CR_FMODE;
        cr |= XSPI_CR_EN;
        xspi->CR = cr;
        { volatile uint32_t _t = XIP_TO_SHORT;
          while ((xspi->SR & XSPI_SR_BUSY) && --_t > 0) {} }

        /* ---- Erase sectors ---- */
        cur_addr  = addr;
        remaining = erase_size;
        while (remaining > 0) {
            if (xip_wait_ready(xspi, XIP_TO_ERASE))  { __enable_irq(); return -20; }
            if (xip_send_cmd_instr(xspi, 0x06, XIP_TO_SHORT)) { __enable_irq(); return -21; }
            if (xip_send_cmd_addr(xspi, 0x21, cur_addr, XIP_TO_SHORT)) { __enable_irq(); return -22; }

            cur_addr  += sector_sz;
            remaining -= (remaining >= sector_sz) ? sector_sz : remaining;
        }
        /* Wait for last erase to finish */
        if (xip_wait_ready(xspi, XIP_TO_ERASE)) { __enable_irq(); return -23; }

        /* ---- Page Program ---- */
        src       = data;
        cur_addr  = addr;
        remaining = data_len;
        while (remaining > 0) {
            chunk = (remaining < page_sz) ? remaining : page_sz;

            if (xip_send_cmd_instr(xspi, 0x06, XIP_TO_SHORT)) { __enable_irq(); return -24; }

            /* Step 1: Set FMODE=0 first (indirect write) */
            cr  = xspi->CR;
            cr &= ~(XSPI_CR_FMODE | XSPI_CR_APMS | XSPI_CR_PMM);
            cr |= XSPI_CR_EN;
            xspi->CR = cr;

            /* Step 2: Configure PP command — AR write triggers indirect cmd in FMODE=0,
             *         then DR writes stream the data */
            xspi->CCR = XIP_CCR_INSTR_ADDR_D;
            xspi->TCR = 0;
            xspi->IR  = XIP_INSTR(0x12);     /* Page Program */
            xspi->AR  = cur_addr;
            xspi->DLR = chunk - 1;

            /* Step 3: Write data to DR byte by byte */
            for (uint32_t i = 0; i < chunk; i++) {
                *((__IO uint8_t *)&xspi->DR) = src[i];
            }

            /* Wait for transfer complete */
            { volatile uint32_t _t = XIP_TO_SHORT;
              while ((xspi->SR & XSPI_SR_BUSY) && --_t > 0) {}
              _t = XIP_TO_SHORT;
              while (!(xspi->SR & XSPI_SR_TCF) && --_t > 0) {} }
            xspi->FCR = XSPI_FCR_CTCF;

            /* Wait for program to finish */
            if (xip_wait_ready(xspi, XIP_TO_PROG)) { __enable_irq(); return -25; }

            cur_addr  += chunk;
            remaining -= chunk;
            src       += chunk;
        }

        /* ---- Restore memory-mapped mode ---- */
        xspi->CCR  = save_ccr;
        xspi->TCR  = save_tcr;
        xspi->IR   = save_ir;
        xspi->WCCR = save_wccr;
        xspi->WTCR = save_wtcr;
        xspi->WIR  = save_wir;

        cr  = xspi->CR;
        cr &= ~XSPI_CR_FMODE;
        cr |= (3U << XSPI_CR_FMODE_Pos) | XSPI_CR_EN;  /* FMODE=3 (MM) */
        xspi->CR = cr;

        /* Re-enable I-Cache */
        SCB->CCR |= SCB_CCR_IC_Msk;
        __DSB();
        __ISB();
        __enable_irq();
        return NVSTORE_OK;
    }
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
