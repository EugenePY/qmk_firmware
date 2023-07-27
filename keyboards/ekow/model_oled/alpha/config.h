/*
Copyright 2022 eugenepy

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include "config_common.h"
#include "oled_config.h"

/* USB Device descriptor parameter */
#define VENDOR_ID 0x4B59  // KY
#define PRODUCT_ID 0x4d4f // MO
#define DEVICE_VER 0x000
#define MANUFACTURER "PlayKeyboradxKeeBoyzLab"
#define PRODUCT "ModelOLED"
#define SERIAL_NUMBER "playkeyboad:model-oled"

// key matrix size
#define MATRIX_ROWS 6
#define MATRIX_COLS 17

// ModelOLED default matrix config
#define MATRIX_ROW_PINS \
    { B8, B9, C15, B6, B5, B7 }

#define MATRIX_COL_PINS \
    { B2, B10, B13, B12, B15, B14, A9, B4, A1, A2, A3, A4, A15, A8, A6, B3, A0 } 

#define USB_MAX_POWER_CONSUMPTION 500

// Wear-leveling Embeded Flash Config
//#define WEAR_LEVELING_LEGACY_EMULATION_FLASH_BASE (uint32_t)(&__eeprom_enu_address__)

//#define WEAR_LEVELING_LOGICAL_SIZE 2048
//#define WEAR_LEVELING_BACKING_SIZE 16384

/*
#* Keyboard Matrix Assignments
#*
#* Change this to how you wired your keyboard
#* COLS: AVR pins used for columns, left to right
#* ROWS: AVR pins used for rows, top to bottom
#* DIODE_DIRECTION: COL2ROW = COL = Anode (+), ROW = Cathode (-, marked on diode)
#*                  ROW2COL = ROW = Anode (+), COL = Cathode (-, marked on diode)
#*
#*/
/* COL2ROW, ROW2COL */

#define DIODE_DIRECTION COL2ROW
#define DEBOUNCE 3

#ifdef SOLENOID_ENABLE
#    define SOLENOID_PIN A10
#    define SOLENOID_ACTIVE true
#    define SOLENOID_DEFAULT_DWELL 75
#endif
