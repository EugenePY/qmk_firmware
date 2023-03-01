/* Storge Abstract Layer
 *
 */
#include <string.h>
#include "flash_ioblock.h"
#include "vfat.h"

static FLASHDriver  driver;
static FLASHDriver *driver_ptr = (void *)NULL;

/* Forward declarations required by flash_vmt.*/
static bool flash_is_inserted(void *instance);
static bool flash_is_write_protected(void *instance);
static bool flash_connect(void *instance);
static bool flash_disconnect(void *instance);
static bool flash_connect(void *instance);
static bool flash_read(void *instance, uint32_t startblk, uint8_t *buffer, uint32_t n);
static bool flash_write(void *instance, uint32_t startblk, const uint8_t *buffer, uint32_t n);
static bool flash_sync(void *instance);
static bool flash_get_info(void *instance, BlockDeviceInfo *bdip);

/**
 * @brief   Virtual methods table.
 */
static const flash_block_device_vmt flash_vmt = {(size_t)0, // object offsets
                                                            // removable media detecthion.
                                                 flash_is_inserted,
                                                 // removable write protection

                                                 flash_is_write_protected,
                                                 // connection to the block device.
                                                 flash_connect,
                                                 // disconnection from the block device.
                                                 flash_disconnect,
                                                 // read from the block device
                                                 flash_read,
                                                 // write to the block device
                                                 flash_write,
                                                 // write async
                                                 flash_sync,
                                                 // media info.
                                                 flash_get_info};

static bool flash_is_inserted(void *instance) {
    return true;
}
static bool flash_is_write_protected(void *instance) {
    return false;
}
static bool flash_connect(void *instance) {
    return true;
}
static bool flash_disconnect(void *instance) {
    return true;
};

bool flash_read(void *instance, uint32_t startblk, uint8_t *buffer, uint32_t n) {
    uint32_t address = FLASH_ADDR(startblk * SECTOR_SIZE_BYTES);
    memcpy(buffer, (uint8_t *)address, n * SECTOR_SIZE_BYTES);
    return true;
}
static bool flash_write(void *instance, uint32_t startblk, const uint8_t *buffer, uint32_t n) {
    uint32_t address = FLASH_ADDR(startblk * SECTOR_SIZE_BYTES);
    RFLASH   res     = flashWrite((flashaddr_t)address, (char *)buffer, n * SECTOR_SIZE_BYTES);
    return res == FLASH_RETURN_SUCCESS;
}
static bool flash_sync(void *instance) {
    return true;
};

static bool flash_get_info(void *instance, BlockDeviceInfo *bdip) {
    bdip->blk_size = SECTOR_SIZE_BYTES;
    bdip->blk_num  = LUN_MEDIA_BLOCKS;
    return true;
}

void flashInit(void) {}

void flashObjectInit(FLASHDriver *driver_p) {
    if (!driver_ptr) {
        driver_p->vmt   = &flash_vmt;
        driver_p->state = BLK_READY;
        driver_ptr      = driver_p;
    }
}
FLASHDriver *get_flashObject(void) {
    if (driver_ptr) {
        return driver_ptr;
    } else {
        flashObjectInit(&driver);
        return driver_ptr;
    }
}
