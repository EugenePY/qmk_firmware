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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "beta.h"
#include "wait.h"
#include "usb_util.h"

#ifdef OLED_ENABLE
#    include "oled_main.h"
#endif

#ifdef SEMIHOST_ENABLE
#    define SEMIHOSTING_SYS_WRITE0 0x04
static inline int32_t semihosting_call(int32_t R0, int32_t R1) {
    int32_t rc;
    __asm__ volatile("mov r0, %1\n" /* move int R0 to register r0 */
                     "mov r1, %2\n" /* move int R1 to register r1 */
                     "bkpt #0xAB\n" /* thumb mode semihosting call */
                     "mov %0, r0"   /* move register r0 to int rc */
                     : "=r"(rc)
                     : "r"(R0), "r"(R1)
                     : "r0", "r1", "ip", "lr", "memory", "cc");
    return rc;
}
static void semihosting_write_string(char *string) {
    semihosting_call(SEMIHOSTING_SYS_WRITE0, (uintptr_t)string);
}

extern void initialise_monitor_handles(void);
#endif

void keyboard_pre_init_user(void) {
#ifdef SEMIHOST_ENABLE
    semihosting_write_string("hello world!\n");
#endif
#ifdef OLED_ENABLE
    oled_task_init();
#endif
}

void early_hardware_init_post(void) {
#ifdef SEMIHOST_ENABLE
    initialise_monitor_handles();
#endif

#ifdef OLED_ENABLE
    if_requested_model_oled_flash();
#endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_IMG:
#ifdef OLED_ENABLE
            model_oled_flash_img_jump();
#endif
        default:
            return true; // Process all other keycodes normally
    }
    return true;
}
