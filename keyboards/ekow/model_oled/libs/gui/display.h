#pragma once
#include "graphic.h"
#include <stdint.h>
#include <ch.h>
#include <stdlib.h>

#define WIDTH 64
#define HEIGHT 48
#define BUFFER_SIZE 512 // NUMBER of PIXEL
#define FRAME_SIZE (WIDTH * HEIGHT)
#define FRAME_BUFFER_SIZE WIDTH* HEIGHT * sizeof(PIXEL_TYPE)
#define RENDER_BUFFER_SIZE BUFFER_SIZE * sizeof(PIXEL_TYPE)

typedef struct {
    uint16_t    cursor;              // pixel cursor of remote display.
    PIXEL_TYPE* buffer; //
    mutex_t*    display_mutex;       // for display
    bool is_on;
    bool rendering;
} display_driver_t;

bool display_init(display_driver_t** self);
bool display_reset(display_driver_t* self); // if the display buffer is currupt this can reset the display without powering off the kb.
void display_on(display_driver_t *self);
void display_off(display_driver_t *self);
void display_render_mode_on(display_driver_t *self);
void disply_buffer_send(display_driver_t* self, size_t size);
