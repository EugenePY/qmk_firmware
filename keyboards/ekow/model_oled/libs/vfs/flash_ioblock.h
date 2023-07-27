/* qmk-chibios flash interface
 */
#pragma once
#include "ch.h"
#include "hal.h"
#include <hal_ioblock.h>
#include "flash.h"

// Flash setup
#define FLASH_ADDR(offset) (FLASH_BASE_ADDR + (offset))
#define FLASH_PTR(offset) ((__IO uint8_t *)FLASH_ADDR(offset))
#define FLASH_BYTE(loc, offset) (*(FLASH_PTR(((uint32_t)loc) + ((uint32_t)offset))))
#define VALID_ADDR(addr) (addr < FLASH_END_ADDR) | (FLASH_BASE_ADDR <= addr)


/**
 * @extends FlashBlockDevice
 *
 * @brief   Structure representing a EEPROM/SD over SPI driver.
 */

#define _flash_block_device_methods _base_block_device_methods

#define _flash_block_device_data _base_block_device_data

typedef struct {
    _flash_block_device_methods
} flash_block_device_vmt;

typedef struct {
    /** @brief Virtual Methods Table.*/
    const flash_block_device_vmt *vmt;
    _flash_block_device_data
} FLASHDriver;

#ifdef __cplusplus
extern "C" {
#endif

void         flashInit(void);
void         flashObjectInit(FLASHDriver *driver);
FLASHDriver *get_flashObject(void);

#ifdef __cplusplus
}
#endif
