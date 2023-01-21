/* default function/MACRO variables declarations */
#pragma once
#include <ch.h>
#include <hal.h>

#include "quantum.h"

#define FLASHING_EVT EVENT_MASK(0)
#define STOP_RENDER_EVT EVENT_MASK(1)

// reset and jump to the entry function
void model_oled_flash_img_jump(void);

void if_requested_model_oled_flash(void);


// OLED Driver Tasks
// OLED task initialization
void oled_task_init(void);

// OLED proccess Record function
bool oled_process_record(uint16_t keycode, keyrecord_t *record);

// OLED task stop
void oled_task_stop(void);

// reset to default image from eeprom
void model_oled_reset_img(void);
