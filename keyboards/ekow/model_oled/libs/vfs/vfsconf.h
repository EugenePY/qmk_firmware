/* Config of Virtual File system for image replacement locate in EEPROM.
 */
#pragma once
#include_next "vfsconf.h"

extern uint32_t __oled_img_base_address__; // this is define in the linker script
extern uint32_t __oled_img_end_address__;

#define FLASH_BASE_ADDR ((uint32_t)(&__oled_img_base_address__))
#define FLASH_END_ADDR  ((uint32_t)(&__oled_img_end_address__))

