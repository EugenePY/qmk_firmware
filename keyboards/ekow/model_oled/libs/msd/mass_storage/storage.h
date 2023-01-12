/* qmk-chibios eeprom interface
 */
#pragma once
#include "ch.h"
#include "hal.h"
#include <hal_ioblock.h>
#include "eeprom.h"

/**
 * @extends EEpromBlockDevice
 *
 * @brief   Structure representing a EEPROM/SD over SPI driver.
 */

#define _eeprom_block_device_methods _base_block_device_methods

#define _eeprom_block_device_data _base_block_device_data

typedef struct {
    _eeprom_block_device_methods
} eeprom_block_device_vmt;

typedef struct {
    /** @brief Virtual Methods Table.*/
    const eeprom_block_device_vmt *vmt;
    _eeprom_block_device_data
} EEPROMDriver;

#ifdef __cplusplus
extern "C" {
#endif
void  eepromInit(void);
void  eepromObjectInit(EEPROMDriver *driver);
#ifdef __cplusplus
}
#endif
