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

<<<<<<< Updated upstream
/* USB Device descriptor parameter */
#define VENDOR_ID 0x4B59  // KY
#define PRODUCT_ID 0x4d4f // MO
#define DEVICE_VER 0x000
#define MANUFACTURER "PlayKeyboradxKeeBoyzLab"
#define PRODUCT "ModelOLED"
=======
>>>>>>> Stashed changes
#define SERIAL_NUMBER "playkeyboad:model-oled"

// key matrix size
#define MATRIX_ROWS 5
#define MATRIX_COLS 17

// ModelOLED default matrix config
#define MATRIX_ROW_PINS \
    { C13, C14, C15, B7, A6}
#define MATRIX_COL_PINS \
<<<<<<< Updated upstream
    { B0, B10, B12, A10, B2, A8, A2, B14, B15, A15, B3, B5, B6, B9, B8, A0, A1}


#define USB_MAX_POWER_CONSUMPTION 500

// Wear-leveling Embeded Flash Config
/* internal eeprom  size */
#ifdef OLED_ENABLE
#    define EECONFIG_USER_DATA_SIZE OLED_DISPLAY_HEIGHT* OLED_DISPLAY_WIDTH
#endif

=======
    { B2, B10, B13, B12, B15, B14, A10, B4, A1, A2, A3, A4, A15, A8, A6, B3, A0 }

#define USB_MAX_POWER_CONSUMPTION 500

>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
#    define SOLENOID_PIN B4
=======
#    define SOLENOID_PIN A9
>>>>>>> Stashed changes
#    define SOLENOID_ACTIVE true
#    define SOLENOID_DEFAULT_DWELL 75
#endif

