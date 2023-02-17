#include "storage.h"
#include "vfat.h"
#include "wear_leveling.h"

/* Forward declarations required by eeprom_vmt.*/
static bool eeprom_is_card_inserted(void *instance);
static bool eeprom_is_write_protected(void *instance);
static bool eeprom_connect(void *instance);
static bool eeprom_disconnect(void *instance);
static bool eeprom_connect(void *instance);
static bool eeprom_read(void *instance, uint32_t startblk, uint8_t *buffer, uint32_t n);
static bool eeprom_write(void *instance, uint32_t startblk, const uint8_t *buffer, uint32_t n);
static bool eeprom_sync(void *instance);
static bool eeprom_get_info(void *instance, BlockDeviceInfo *bdip);

/**
 * @brief   Virtual methods table.
 */
static const eeprom_block_device_vmt eeprom_vmt = {(size_t)0, // object offsets
                                                              // removable media detecthion.
                                                   eeprom_is_card_inserted,
                                                   // removable write protection
                                                   eeprom_is_write_protected,
                                                   // connection to the block device.
                                                   eeprom_connect,
                                                   // disconnection from the block device.
                                                   eeprom_disconnect,
                                                   // read from the block device
                                                   eeprom_read,
                                                   // write to the block device
                                                   eeprom_write,
                                                   // write async
                                                   eeprom_sync,
                                                   // media info.
                                                   eeprom_get_info};

static bool eeprom_is_card_inserted(void *instance) {
    return true;
}
static bool eeprom_is_write_protected(void *instance) {
    return false;
}
static bool eeprom_connect(void *instance) {
    return true;
}
static bool eeprom_disconnect(void *instance) {
    return true;
};

static bool eeprom_read(void *instance, uint32_t startblk, uint8_t *buffer, uint32_t n) {
    for (uint16_t i = 0; i < n; i++) {
        vfs_read_fat12(startblk + i, buffer++);
    }
    return true;
}
static bool eeprom_write(void *instance, uint32_t startblk, const uint8_t *buffer, uint32_t n) {
    for (uint16_t i = 0; i < n; i++) {
        vfs_write_fat12(startblk + i, (uint8_t *)buffer);
    }
    return true;
}
static bool eeprom_sync(void *instance) {
    return true;
};

static bool eeprom_get_info(void *instance, BlockDeviceInfo *bdip) {
    bdip->blk_size = SECTOR_SIZE_BYTES;
    bdip->blk_num  = LUN_MEDIA_BLOCKS;
    return true;
}

void eepromInit(void) {
}

void eepromObjectInit(EEPROMDriver *driver) {
    driver->vmt   = &eeprom_vmt;
    driver->state = BLK_READY;
}
