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

#include "quantum.h"

#if defined(KEYBOARD_ekow_model_oled_alpha)
#    include "alpha.h"
#elif defined(KEYBOARD_ekow_model_oled_beta)
#    include "beta.h"
#elif defined(KEYBOARD_ekow_model_oled_gamma)
#    include "gamma.h"
#endif

typedef union {
    uint32_t raw;
    struct {
        bool   img_is_empty : 1;
        bool   is_dirty : 1;
        size_t n_frame : 1;
    };
} user_config_t;
