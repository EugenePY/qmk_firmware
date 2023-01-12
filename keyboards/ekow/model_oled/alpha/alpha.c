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
#include "alpha.h"
#include "stdbool.h"

#ifdef SOLENOIDE_ENABLE
#    include "solenoid.h"
#endif

#ifdef OLED_ENABLE
#    include "ssd1331.h"
#    include "img/icon.h"

bool oled_task_kb(void) {
    // render image data
    oled_render();
    return true;
}

#endif

void keyboard_pre_init_user(void) {
    // Call the keyboard pre init code.
#ifdef SOLENOIDE_ENABLE
    solenoid_init();
#endif

#ifdef OLED_ENABLE
    // current only support ROTAION_0
    oled_init(OLED_ROTATION_0);
#endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        // Do something when pressed
        solenoid_on();
    } else {
        // Do something else when release
        solenoid_off();
    }
    switch (keycode) {
        case KC_SOL:
            solenoid_driver_toggle();
            return false; // Skip all further processing of this key
    }
    return true;
}
