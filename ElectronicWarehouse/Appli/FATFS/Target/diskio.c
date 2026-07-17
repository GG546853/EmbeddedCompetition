/**
 ****************************************************************************************************
 * @file        diskio.c
 * @brief       FATFS disk I/O layer for SD NAND (SDMMC2)
 *              Single volume — pdrv=0 maps to SD NAND
 ****************************************************************************************************
 */

#include "ff.h"
#include "diskio.h"
#include "sd_nand.h"

/**
 * @brief   Get disk status
 * @param   pdrv : Physical drive number (always 0)
 * @retval  DSTATUS
 */
DSTATUS disk_status(BYTE pdrv)
{
    if (pdrv != 0) return STA_NODISK;
    return RES_OK;
}

/**
 * @brief   Initialize disk
 * @param   pdrv : Physical drive number (always 0)
 * @retval  DSTATUS
 * @note    hsd2 is already initialized by MX_SDMMC2_SD_Init().
 *          sd_nand_init() validates the card is present.
 */
DSTATUS disk_initialize(BYTE pdrv)
{
    if (pdrv != 0) return STA_NODISK;

    if (sd_nand_init() != 0)
        return RES_ERROR;

    return RES_OK;
}

/**
 * @brief   Read sectors
 * @param   pdrv   : Physical drive number (always 0)
 * @param   buff   : Destination buffer
 * @param   sector : Starting sector (LBA)
 * @param   count  : Number of sectors
 * @retval  DRESULT
 */
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != 0) return RES_PARERR;

    if (sd_nand_read_disk(buff, (uint32_t)sector, (uint32_t)count) != 0)
        return RES_ERROR;

    /* D-Cache clean+invalidate: flush dirty lines first so invalidate
       doesn't discard them; then CPU sees DMA data from the card */
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)buff, count * 512);

    return RES_OK;
}

#if FF_FS_READONLY == 0
/**
 * @brief   Write sectors
 * @param   pdrv   : Physical drive number (always 0)
 * @param   buff   : Source buffer
 * @param   sector : Starting sector (LBA)
 * @param   count  : Number of sectors
 * @retval  DRESULT
 */
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != 0) return RES_PARERR;

    /* D-Cache clean: flush CPU writes to memory before DMA reads it */
    SCB_CleanDCache_by_Addr((uint32_t *)buff, count * 512);

    printf("[DISKIO] write sect=%lu cnt=%u\r\n", (unsigned long)sector, (unsigned)count);
    if (sd_nand_write_disk((uint8_t *)buff, (uint32_t)sector, (uint32_t)count) != 0)
        return RES_ERROR;

    return RES_OK;
}
#endif

/**
 * @brief   Miscellaneous I/O control
 * @param   pdrv : Physical drive number (always 0)
 * @param   cmd  : Control command
 * @param   buff : Parameter buffer
 * @retval  DRESULT
 */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    if (pdrv != 0) return RES_PARERR;

    switch (cmd)
    {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_SIZE:
            *(DWORD *)buff = 512;
            return RES_OK;

        case GET_BLOCK_SIZE:
            *(DWORD *)buff = g_sd_nand_info_struct.LogBlockSize;
            return RES_OK;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = g_sd_nand_info_struct.LogBlockNbr;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}
