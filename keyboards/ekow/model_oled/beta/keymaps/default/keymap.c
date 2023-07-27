/* Copyright 2022 eugenepy
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

#include QMK_KEYBOARD_H

user_config_t user_config;

void eeconfig_init_user(void) {
    user_config.raw         = 0x0000;
    user_config.img_is_empty = 1;
    user_config.n_frame = 1;
    user_config.is_dirty = 0;
    eeconfig_update_user(user_config.raw);
}

// Default layer
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = FOUR(KC_TOGGLE_SOLENODE, MO(1), KC_ENTER_FLASH_IMG, KC_Q),
    [1] = FOUR(KC_B, KC_D, KC_E, KC_F)
};
