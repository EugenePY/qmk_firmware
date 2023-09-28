/* Copyright 2020-2022 Gondolindrim
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

#pragma once

#include "stdio.h"
#include "stdint.h"

#include "quantum.h"
#include "qp_ssd1331.h"

#if defined(KEYBOARD_ekow_model_oled_alpha)
#    include "alpha.h"
#elif defined(KEYBOARD_ekow_model_oled_beta)
#    include "beta.h"
#endif

typedef union {
    uint32_t raw;
    struct {
        bool    timeout_enable : 1;
        uint8_t n_image : 2;
    };
} oled_config_t;

enum OLED_80_KEYCODES { KC_ENTER_FLASH_IMG = QK_KB, KC_OLED_TOGGLE };

#define KC_IMG KC_ENTER_FLASH_IMG

// some functions declaration
extern void platform_setup(void);
extern void protocol_setup(void);
extern void protocol_pre_init(void);
extern void protocol_post_init(void);
extern void protocol_pre_task(void);
extern void protocol_post_task(void);
extern void protocol_init(void);
extern void protocol_task(void);

// idle function
uint16_t get_timeout_threshold(void);
void     timeout_reset_timer(void);

// eeprom config
void oled_read_config(void);

void graphic_forward_kb(void);
void graphic_backward_kb(void);
void render_graphic(painter_image_handle_t graphic);
