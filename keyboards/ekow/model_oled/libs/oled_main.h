/* default function/MACRO variables declarations */
#pragma once
#include <ch.h>
#include <stdint.h>
#include <hal.h>

#include "quantum.h"

#define FLASHING_EVT EVENT_MASK(0)
#define STOP_RENDER_EVT EVENT_MASK(1)

// reset and jump to the entry function
void model_oled_flash_img_jump(void);

bool if_requested_model_oled_flash(void);

