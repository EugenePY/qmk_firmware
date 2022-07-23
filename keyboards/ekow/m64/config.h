/*
Copyright 2022 Eugene-Yuan Kou

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

/* USB Device descriptor parameter */
#define VENDOR_ID 0x4B59 // KY
#define PRODUCT_ID 0x0000
#define DEVICE_VER 0x0001
#define MANUFACTURER KeeBoyzLab
#define PRODUCT M64

// key matrix size
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

// M64 PCB default pin-out
#define MATRIX_ROW_PINS \
    { B10, A15, B1, B0, A4 }
#define MATRIX_COL_PINS \
    { A1, B12, A8, B13, A10, A13, B4, B3, B5, B8, B9, C13, C14, C15, A0 }

// Keyboard Matrix Assignments
#define DIODE_DIRECTION COL2ROW
#define DEBOUNCE 5

// RGB Matrix
#define DRIVER_COUNT 1
#define DRIVER_ADDR_1 0b0110000
#define LED_DRIVER_1_LED_TOTAL 1
#define DRIVER_LED_TOTAL LED_DRIVER_1_LED_TOTAL
#define ISSI_DRIVER_TOTAL DRIVER_LED_TOTAL

#define RGB_MATRIX_STARTUP_VAL 80
#define RGB_MATRIX_STARTUP_MODE RGB_MATRIX_SOLID_COLOR

// RGB Colors

/* I2C config
 * M64 using the I2C1
 */
// RGB Underglow
/* M64 using PWD to control the underglow.
 */
#define RGB_DI_PIN B15
#define RGBLIGHT_EFFECT_RGB_TEST

#define WS2812_PWM_COMPLEMENTARY_OUTPUT
#define WS2812_PWM_DRIVER PWMD1
#define WS2812_PWM_CHANNEL 3

#define WS2812_PWM_PAL_MODE 1
#define WS2812_DMA_STREAM STM32_DMA2_STREAM5
#define WS2812_DMA_CHANNEL 6

// Indicator LED
// define the is31f3741 pin

// LAYER LED
//#define LAYER_INDICATOR_ENABLE

// CAPsLock
#define CAP_INDICATOR_ENABLE
