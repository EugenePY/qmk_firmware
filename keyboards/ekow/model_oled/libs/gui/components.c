#include <stdlib.h>
#include <stdbool.h>
#include <lib/lib8tion/lib8tion.h>

#include "graphic.h"
#include "components.h"
#include "display.h"

static void render(void* _self, PIXEL_TYPE* buffer, uint16_t start, uint16_t end) {
    component_t* self     = _self;
    idx_2d_t     start_2d = {.x = self->data->x, .y = self->data->y};
    idx_2d_t     end_2d   = {.x = self->data->x + self->data->width - 1, .y = self->data->y + self->data->height - 1};

    for (uint16_t i = start; i < end; i++) {
        idx_2d_t idx_2d = IDX_2D(i, WIDTH);
        if (((idx_2d.x >= start_2d.x) & (idx_2d.x <= end_2d.x)) & ((idx_2d.y >= start_2d.y) & (idx_2d.y <= end_2d.y))) {
            buffer[i - start] = DEFAULT_BG;
        }
    }
}

static void event_cb(void* self, eventflags_t event) {
    switch (event) {
        default:
            break;
    }
}

static component_vmt compoment_vm_ = {.render = render, .event_cb = event_cb};

bool ComponentInit(void** _self, component_data_t* data) {
    component_t* self = *_self;
    if (NULL == (self = malloc(sizeof(component_t)))) return false;
    (self)->data = data;
    (self)->vm   = &compoment_vm_;
    *_self       = self;
    return true;
}

typedef void (*frame_render_func_t)(frame_t*, PIXEL_TYPE* buffer, uint16_t start, uint16_t end);

/* get the linear buffer from frame */
static void frame_render(void* _self, PIXEL_TYPE* buffer, uint16_t start, uint16_t end) {
    frame_t* self = _self;
    // the input frame is located at position x, y
    idx_2d_t start_2d = IDX_2D(start, SUPER(self)->data->width);
    idx_2d_t end_2d   = IDX_2D(end, SUPER(self)->data->width);

    for (uint16_t i = start; i < end; i++) {
        idx_2d_t idx_2d = IDX_2D(i, SUPER(self)->data->width);
        if ((idx_2d.x > start_2d.x) | (idx_2d.x < end_2d.x) | (idx_2d.y > start_2d.y) | (idx_2d.y < end_2d.y)) {
            buffer[i] = get_pixel(self->background_image, IDX(idx_2d.x, idx_2d.y, SUPER(self)->data->width));
        }
    }
}

bool FrameInit(void** _self, component_data_t* data, HashMap* background_image) {
    frame_t* self = *_self;
    if (NULL == (self = malloc(sizeof(frame_t)))) return false;
    if (!ComponentInit((void**)SUPER(self), data)) return false; // inherentance
    // override the default method
    self->vm                 = SUPER(self)->vm;
    self->vm->render         = frame_render; //
    self->data               = SUPER(self)->data;
    (self)->background_image = background_image;
    return true;
}


/* given two start frame and end frame
 * total frame: total frame require to complete the transition
 */
// bool TransitionInit(void** _self, component_t* target, uint16_t distance_left);

// static void transition_component(transition_t* self, component_t* target, uint16_t shift_x, uint16_t shift_y) {
//    uint16_t v = (self->distance_left * self->distance_left);
//};
