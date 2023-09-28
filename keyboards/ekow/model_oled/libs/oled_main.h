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


// OLED Driver Tasks
// OLED task initialization
void oled_task_init(void);

// OLED proccess Record function
bool oled_process_record(uint16_t keycode, keyrecord_t *record);

// OLED task stop
void oled_task_stop(void);

// reset to default image from eeprom
void model_oled_reset_img(void);
<<<<<<< HEAD
=======
<<<<<<< Updated upstream
=======
>>>>>>> model-m-painter

typedef struct {
    uint8_t n_frame;
    uint32_t time_delay;
} __attribute__((packed)) image_header_t;

typedef struct {
    image_header_t header;
    uint8_t *img_buffer;
} __attribute__((packed)) image_file_t;

void img_init(void);
<<<<<<< HEAD
//void img_read(image_meta_t*);
=======
>>>>>>> Stashed changes
>>>>>>> model-m-painter
