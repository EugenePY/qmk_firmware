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
    {0, CS12_SW1, CS11_SW1, CS10_SW1}, /* RGB1 */
};

void set_color(rgb_color_t *color, uint8_t r, uint8_t g, uint8_t b) {
    color->r = r;
    color->g = g;
    color->b = g;
}

led_config_t g_led_config = {{
                                 /* C0       C1       C2       C3       C4       C5       C6       C7       C8       C9       C10       C11       C12       C13       C14       C15 */
                                 {0},
                             },
                             {
                                 {0, 0} // 15
                             },
                             {8}};
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
