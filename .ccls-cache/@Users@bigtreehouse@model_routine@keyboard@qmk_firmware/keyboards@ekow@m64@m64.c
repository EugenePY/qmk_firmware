/* Copyright 2021 eugenepy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "m64.h"

#ifdef CONSOLE_ENABLE
#    include "debug.h"
#endif

#ifdef RGB_MATRIX_ENABLE

void board_init(void) {
    setPinOutput(ISS_SDB_PIN);
    writePinHigh(ISS_SDB_PIN);
}

const is31_led PROGMEM g_is31_leds[DRIVER_LED_TOTAL] = {
    {0, CS27_SW8, CS26_SW8, CS25_SW8},
    {0, CS27_SW1, CS26_SW1, CS25_SW1},
    {0, CS27_SW2, CS26_SW2, CS25_SW2},
    {0, CS27_SW3, CS26_SW3, CS25_SW3},
    {0, CS27_SW4, CS26_SW4, CS25_SW4},
    {0, CS27_SW5, CS26_SW5, CS25_SW5},
    {0, CS27_SW6, CS26_SW6, CS25_SW6},
    {0, CS27_SW7, CS26_SW7, CS25_SW7},
    {0, CS2_SW8, CS3_SW8, CS1_SW8},
    {0, CS2_SW1, CS3_SW1, CS1_SW1},
    {0, CS2_SW2, CS3_SW2, CS1_SW2},
    {0, CS2_SW3, CS3_SW3, CS1_SW3},
    {0, CS2_SW4, CS3_SW4, CS1_SW4},
    {0, CS2_SW5, CS3_SW5, CS1_SW5},
    {0, CS2_SW6, CS3_SW6, CS1_SW6},
    {0, CS2_SW7, CS3_SW7, CS1_SW7}, {0, CS30_SW8, CS29_SW8, CS28_SW8}, {0, CS30_SW1, CS29_SW1, CS28_SW1},
    {0, CS30_SW2, CS29_SW2, CS28_SW2},
    {0, CS30_SW3, CS29_SW3, CS28_SW3},
    {0, CS30_SW4, CS29_SW4, CS28_SW4},
    {0, CS30_SW5, CS29_SW5, CS28_SW5},
    {0, CS30_SW6, CS29_SW6, CS28_SW6},
    {0, CS30_SW7, CS29_SW7, CS28_SW7},
    {0, CS5_SW8, CS6_SW8, CS4_SW8},
    {0, CS5_SW1, CS6_SW1, CS4_SW1},
    {0, CS5_SW2, CS6_SW2, CS4_SW2},
    {0, CS5_SW3, CS6_SW3, CS4_SW3},
    {0, CS5_SW4, CS6_SW4, CS4_SW4},
    {0, CS5_SW5, CS6_SW5, CS4_SW5},
    {0, CS5_SW6, CS6_SW6, CS4_SW6},
    {0, CS36_SW8, CS35_SW8, CS34_SW8},
    {0, CS36_SW1, CS35_SW1, CS34_SW1},
    {0, CS36_SW2, CS35_SW2, CS34_SW2},
    {0, CS36_SW3, CS35_SW3, CS34_SW3},
    {0, CS36_SW4, CS35_SW4, CS34_SW4},
    {0, CS36_SW5, CS35_SW5, CS34_SW5},
    {0, CS36_SW6, CS35_SW6, CS34_SW6},
    {0, CS36_SW7, CS35_SW7, CS34_SW7},
    {0, CS16_SW8, CS15_SW8, CS14_SW8},
    {0, CS16_SW1, CS15_SW1, CS14_SW1},
    {0, CS16_SW2, CS15_SW2, CS14_SW2},
    {0, CS16_SW3, CS15_SW3, CS14_SW3},
    {0, CS16_SW4, CS15_SW4, CS14_SW4},
    {0, CS16_SW5, CS15_SW5, CS14_SW5},
    {0, CS33_SW8, CS32_SW8, CS31_SW8},
    {0, CS33_SW1, CS32_SW1, CS31_SW1},
    {0, CS33_SW2, CS32_SW2, CS31_SW2},
    {0, CS33_SW3, CS32_SW3, CS31_SW3},
    {0, CS33_SW4, CS32_SW4, CS31_SW4},
    {0, CS33_SW5, CS32_SW5, CS31_SW5},
    {0, CS33_SW6, CS32_SW6, CS31_SW6},
    {0, CS33_SW7, CS32_SW7, CS31_SW7},
    {0, CS10_SW8, CS9_SW8, CS8_SW8},
    {0, CS10_SW1, CS9_SW1, CS8_SW1},
    {0, CS10_SW2, CS9_SW2, CS8_SW2},
    {0, CS10_SW3, CS9_SW3, CS8_SW3},
    {0, CS10_SW4, CS9_SW4, CS8_SW4},
    {0, CS10_SW5, CS9_SW5, CS8_SW5},
    {0, CS10_SW6, CS9_SW6, CS8_SW6},
    {0, CS24_SW8, CS23_SW8, CS22_SW8},
    {0, CS24_SW1, CS23_SW1, CS22_SW1},
    {0, CS24_SW2, CS23_SW2, CS22_SW2},
    {0, CS24_SW3, CS23_SW3, CS22_SW3},
    {0, CS24_SW6, CS23_SW6, CS22_SW6},
    {0, CS13_SW2, CS12_SW2, CS11_SW2},
    {0, CS13_SW3, CS12_SW3, CS11_SW3},
    {0, CS13_SW4, CS12_SW4, CS11_SW4},
    {0, CS13_SW5, CS12_SW5, CS11_SW5},
    {0, CS13_SW6, CS12_SW6, CS11_SW6},
};

void set_color(rgb_color_t *color, uint8_t r, uint8_t g, uint8_t b) {
    color->r = r;
    color->g = g;
    color->b = g;
}

led_config_t g_led_config = {
{
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14}, 
    {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30}, 
    {31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 15}, 
    {45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59}, 
    {60, 61, 62, 63, 64, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED,  65, 66, 67, 68, 69}

},
                            {
    {0, 0}, {28, 0}, {41, 0}, {55, 0}, {68, 0}, {82, 0}, 
    {96, 0}, {109, 0}, {123, 0}, {136, 0}, {150, 0}, 
    {163, 0}, {177, 0}, {191, 0}, {204, 0}, {218, 0}, 
    {0, 16}, {28, 16}, {48, 16}, {62, 16}, {75, 16}, 
    {89, 16}, {102, 16}, {116, 16}, {129, 16}, {143, 16}, 
    {157, 16}, {170, 16}, {184, 16}, {197, 16}, {211, 16}, {0, 32}, {28, 32}, {51, 32}, {65, 32}, {79, 32}, {92, 32}, {106, 32}, {119, 32}, {133, 32}, {146, 32}, {160, 32}, {174, 32}, {187, 32}, {201, 32}, {0, 48}, {28, 48}, {58, 48}, {72, 48}, {85, 48}, {99, 48}, {112, 48}, {126, 48}, {140, 48}, {153, 48}, {167, 48}, {180, 48}, {194, 48}, {211, 48}, {224, 48}, {0, 64}, {28, 64}, {45, 64}, {62, 64}, {79, 64}, {163, 64}, {180, 64}, {197, 64}, {211, 64}, {224, 64}
                             },
{8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}};
#endif

#ifdef CONSOLE_ENABLE
void keyboard_post_init_user(void) {
    debug_enable   = true;
    debug_matrix   = true;
    debug_keyboard = true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // If console is enabled, it will print the matrix position and status of each key pressed
    uprintf("KL: kc: %u, col: %u, row: %u, pressed: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed);

    return true;
}
#endif

#if defined(CAP_INDICATOR_ENABLE) || defined(LAYER_INDICATOR_ENABLE)
<<<<<<< Updated upstream
=======
/*
bool led_update_kb(led_t led_state) {
    if (led_update_user(led_state)) {
        if (led_state.caps_lock) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}
*/
>>>>>>> Stashed changes

void rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // Default RED
    rgb_color_t color = {RGB_RED};

    if (host_keyboard_led_state().caps_lock) {
        // GREEN
        set_color(&color, RGB_GREEN);
    }

    for (uint8_t i = led_min; i <= led_max; i++) {
        if (g_led_config.flags[i] == LED_FLAG_INDICATOR) {
            rgb_matrix_set_color(i, color.r, color.g, color.b);
        }
    }
}

#endif
