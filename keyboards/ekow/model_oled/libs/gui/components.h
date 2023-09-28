#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include "graphic.h"
#include <stdio.h>

#define DEFAULT_BG BLACK
// some oo macro
#define SUPER(x) (x)->super
#define COMPONENT_CALL(self, method, ...) ((component_t*)(self))->vm->method(self, __VA_ARGS__) // polymorphic
#define VIRTUALCALL(self, name, ...) SUPER(self)->vm->method(self, __VA_ARGS__)

typedef struct component_s component_t;

typedef struct {
    WIDTH_TYPE  x;      // px, location
    HEIGHT_TYPE y;      // px, location, where the component located
    WIDTH_TYPE  width;  // width of the component
    HEIGHT_TYPE height; // height of the component
    bool        active; //
} component_data_t;

typedef struct {
    void (*render)(void* self, PIXEL_TYPE* buffer, uint16_t start, uint16_t end);
    void (*event_cb)(void* self, eventflags_t event);
} component_vmt;

struct component_s {
    component_data_t* data;
    component_vmt*    vm; // using vmt
};

bool ComponentInit(void** _self, component_data_t* data);
// **

// frame
typedef struct frame_s frame_t;

struct frame_s {
    component_data_t* data; // virtual
    component_vmt*    vm;   // virtual
    component_t*      super;
    HashMap*          background_image;
};

/* Transition */
typedef struct transition_s transition_t;

struct transition_s {
    component_data_t* data; // virtual
    component_vmt*    vm;   // virtual

    component_t* target;
    uint16_t     distance_left;
    uint16_t     accelerate;
    uint16_t     mass;
    uint16_t     n;
    bool         active; // if active its transitioning
    void (*tranistion)(transition_t* self, component_t* target, uint16_t shift_x);
};

#endif
