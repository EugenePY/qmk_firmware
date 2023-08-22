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

#include "oled_config.h"

/* USB Device descriptor parameter */
#define VENDOR_ID 0x4B59  // KY
#define PRODUCT_ID 0x4d4f // MO
#define DEVICE_VER 0x000
#define MANUFACTURER "PlayKeyboradxKeeBoyzLab"
#define PRODUCT "ModelOLED"
#define SERIAL_NUMBER "playkeyboad:model-oled"

// key matrix size
#define MATRIX_ROWS 2
#define MATRIX_COLS 2

// ModelOLED default matrix config
#define MATRIX_ROW_PINS \
    { B8, B9 }

#define MATRIX_COL_PINS \
    { B4, B10}

#define USB_MAX_POWER_CONSUMPTION 500

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
#    define SOLENOID_PIN C13
#    define SOLENOID_ACTIVE true
#    define SOLENOID_DEFAULT_DWELL 75
#endif

/* USB endpoint settings
 * Shard endpoints between mouse key and normal kbs.
 */


