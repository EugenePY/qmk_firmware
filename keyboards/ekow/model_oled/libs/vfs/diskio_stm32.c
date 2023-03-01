/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <stdbool.h>
#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "flash_ioblock.h"
#include "vfat.h"

/* Definitions of physical drive number for each drive */
#define DEV_FLASH 0 /* Example: Map Ramdisk to physical drive 0 */
static FLASHDriver *driver_ptr = (void *)NULL;
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
    switch (pdrv) {
        case DEV_FLASH:
            if (!driver_ptr)
                return STA_NOINIT;
            else
                switch (driver_ptr->state) {
                    case BLK_READY:
                        return RES_OK;
                    default:
                        return RES_ERROR;
                }
    }
    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
    switch (pdrv) {
        case DEV_FLASH:
            driver_ptr = get_flashObject();
            return RES_OK;
    }
    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE  pdrv,   /* Physical drive nmuber to identify the drive */
                  BYTE *buff,   /* Data buffer to store read data */
                  LBA_t sector, /* Start sector in LBA */
                  UINT  count   /* Number of sectors to read */
) {
    bool res = false;
    switch (pdrv) {
        case DEV_FLASH:
            res = blkRead(driver_ptr, sector, buff, count);
    }
    if (res) return RES_OK;
    return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE        pdrv,   /* Physical drive nmuber to identify the drive */
                   const BYTE *buff,   /* Data to be written */
                   LBA_t       sector, /* Start sector in LBA */
                   UINT        count   /* Number of sectors to write */

) {
    bool res = false;
    switch (pdrv) {
        case DEV_FLASH:
            res = blkWrite(driver_ptr, sector, buff, count);
            if (res) return RES_OK;
    }

    return RES_PARERR;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE  pdrv, /* Physical drive nmuber (0..) */
                   BYTE  cmd,  /* Control code */
                   void *buff  /* Buffer to send/receive control data */
) {
    BlockDeviceInfo bdip;
    switch (pdrv) {
        case DEV_FLASH:
            blkGetInfo(driver_ptr, &bdip);
            (void)buff;
            switch (cmd) {
                case CTRL_SYNC:
                    return RES_OK;
                case GET_SECTOR_COUNT:
                    *((DWORD *)buff) = bdip.blk_num;
                    return RES_OK;
                case GET_SECTOR_SIZE:
                    *((WORD *)buff) = bdip.blk_size;
                    return RES_OK;
                default:
                    return RES_PARERR;
            }
    }

    return RES_PARERR;
}

DWORD get_fattime(void) {
    return ((uint32_t)0 | (1 << 16)) | (1 << 21);
}
