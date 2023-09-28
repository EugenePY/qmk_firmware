#include "display.h"
#include "debug.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "ssd1331.h"

static display_driver_t *driver_pt = NULL;

static bool is_commandlock = false;

static void next_cursor(display_driver_t *self, size_t n) {
    self->cursor += n;
    self->cursor %= FRAME_SIZE;
};

static bool lockcmd(void) {
    const uint8_t command[] = {SSD1331_CMD_COMMANDLOCK, SSD1331_ARG_COMMANDLOCK_LOCK};
    spi_status_t  result    = _command_transaction(command, 1);
    is_commandlock          = true;
    return result == SPI_STATUS_SUCCESS;
}
static bool unlockcmd(void) {
    spi_status_t result = SPI_STATUS_SUCCESS;
    if (is_commandlock) {
        const uint8_t command[] = {SSD1331_CMD_COMMANDLOCK, SSD1331_ARG_COMMANDLOCK_UNLOCK};
        result                  = _command_transaction(command, 1);
        is_commandlock          = false;
    }

    return result == SPI_STATUS_SUCCESS;
}

bool display_init(display_driver_t **self) {
    if (*self) {
        if (NULL == (*self = malloc(sizeof(display_driver_t)))) return false;
    }
    driver_pt                = *self;
    driver_pt->buffer        = malloc(sizeof(PIXEL_TYPE) * BUFFER_SIZE);
    driver_pt->display_mutex = malloc(sizeof(mutex_t));
    driver_pt->cursor        = 0;
    driver_pt->is_on         = false;
    if (NULL == driver_pt->buffer) return false;
    chMtxObjectInit(driver_pt->display_mutex);
    oled_init(0);
    return true;
};
void display_on(display_driver_t *self) {
    if (!self->is_on) {
        oled_on();
        self->is_on = true;
    }
};
void display_off(display_driver_t *self) {
    oled_off();
}

void display_render_mode_on(display_driver_t *self) {
    if (self->rendering) {
        lockcmd();
        self->rendering = true;
    }
}

void display_render_mode_off(display_driver_t *self) {
    if (self->rendering) {
        unlockcmd();
        self->rendering = false;
    }
}

static uint8_t decode_buffer[RENDER_BUFFER_SIZE] = {0x00};

void disply_buffer_send(display_driver_t *self, size_t size) {
    // if (self->cursor == 0) {
    // unlockcmd();
    // ssd1331_oled_setup_window();
    //}
    size_t cliped_size = MIN(size, BUFFER_SIZE);
    // iterate from pixel buffer
    for (int i = 0; i < size; i++) {
        decode_buffer[i * sizeof(PIXEL_TYPE)]     = (uint8_t)(self->buffer[i].raw >> 8);
        decode_buffer[i * sizeof(PIXEL_TYPE) + 1] = (uint8_t)(self->buffer[i].raw);
    }
    ssd1331_oled_render(decode_buffer, cliped_size * sizeof(PIXEL_TYPE));
    memset(driver_pt->buffer, 0x00, RENDER_BUFFER_SIZE);
    next_cursor(self, cliped_size);
}
