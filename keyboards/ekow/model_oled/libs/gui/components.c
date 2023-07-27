#include "components.h"
#include "graphic.h"
#include <stdio.h>
#include <stdbool.h>

static PIXEL_TYPE _render_pixel(component_t* self, idx_t idx) {
    return DEFAULT_BG;
}

static void render(component_t* self, PIXEL_TYPE* buffer, size_t n) {
    for (size_t i = 0; i < n; i++) {
        buffer[i] = DEFAULT_BG;
    }
}

static void event_cb(component_t* self, event_t event) {
    switch (event.event_code) {
        default:
            break;
    }
}
bool init(component_t** self, component_data_t data) {
    if (NULL == (*self = malloc(sizeof(component_t)))) return false;
    (*self)->data         = data;
    (*self)->vm->render   = render;
    (*self)->vm->event_cb = event_cb;
    return true;
}

static PIXEL_TYPE _frame_render_pixel(frame_t* self, idx_t idx) {
}
void frame_render(frame_t* self, PIXEL_TYPE* buffer, size_t n) {
    render(self->super, buffer, n);
}

void frame_event_cb(frame_t* self, event_t event) {}

bool init(frame_t** self, frame_data_t data) {
    if (!init(&((*self)->super), data.super)) return false;
}
