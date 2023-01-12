#pragma once
#include <ch.h>
#include <hal.h>

#include "quantum.h"

#define FLASHING_EVT EVENT_MASK(0)
#define STOP_RENDER_EVT EVENT_MASK(1)

extern event_source_t oled_event_source;

// OLED task initialization
void oled_task_init(void);

// OLED proccess Record function
bool oled_process_record(uint16_t keycode, keyrecord_t *record);

// OLED task stop
void oled_task_sleep(void);

// reset to default image from eeprom
void model_oled_reset_img(void);
