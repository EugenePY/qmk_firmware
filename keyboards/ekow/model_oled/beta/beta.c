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
#include "beta.h"
#include "stdbool.h"
#include "wait.h"
#include "usb_util.h"

#ifdef OLED_ENABLE
#    include "oled_main.h"
#endif

void keyboard_pre_init_user(void) {
#ifdef OLED_ENABLE
    oled_task_init();
#endif
}

void keyboard_post_init_user(void) {
#ifdef CONSOLE_ENABLE
    debug_enable = true;
#endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_IMG:
            chEvtBroadcastFlags(&oled_event_source, FLASHING_EVT);
        default:
            return true; // Process all other keycodes normally
    }
    return true;
}
