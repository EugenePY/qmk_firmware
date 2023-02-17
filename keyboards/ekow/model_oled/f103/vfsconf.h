/* Config of Virtual File system for image replacement locate in EEPROM.
 */
#pragma once
#include_next "vfsconf.h"

extern uint16_t _img_flash_size;
extern uint32_t (_img_flash_base);

#define SECTOR_SIZE_BYTES 256
#define FLASH_FILE_SIZE_BYTES 1024 * 16

