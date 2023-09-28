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

=======
    { B4, B10 }
>>>>>>> Stashed changes

#define USB_MAX_POWER_CONSUMPTION 500

// Wear-leveling Embeded Flash Config
/* internal eeprom  size */
#ifdef OLED_ENABLE
#    define EECONFIG_USER_DATA_SIZE OLED_DISPLAY_HEIGHT* OLED_DISPLAY_WIDTH
#endif

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

<<<<<<< Updated upstream
=======
/* USB endpoint settings
 * Shard endpoints between mouse key and normal kbs.
 */
>>>>>>> Stashed changes
