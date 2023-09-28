/* Copyright 2aa022 eugenepy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public Lcense for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdbool.h>
#include <stdio.h>
#include "beta.h"
#include "graphic.h"
#include "model_oled.h"

bool process_record_keymap(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        case KC_G:
            if (record->event.pressed) {
                graphic_forward_kb();
                return false;
            }
        case KC_A:
            if (record->event.pressed) {
                graphic_backward_kb();
                return false;
            }
        default:
            break;
    }
    return true;
}
