<<<<<<< HEAD
#pragma once
#include "gui.h"
#include "graphic.h"

#define BLACK (PIXEL_TYPE)0x0000U
#define DEFAULT_BG BLACK

typedef struct {
    WIDTH_TYPE  x; // px
    HEIGHT_TYPE y; // px
    app_t*      app_pt;
} component_data_t;

typedef struct component_vm component_vmt;
typedef struct {
    component_data_t data;
    component_vmt*   vm; // using vmt
} component_t;

bool init(component_t** self, component_data_t data);
=======
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
>>>>>>> model-m-painter

// frame
typedef struct frame_s frame_t;

<<<<<<< HEAD
typedef struct {
    component_data_t super;
    compressed_gf_t background_image;
} frame_data_t;

typedef struct {
    component_vmt* super;
} frame_vmt_t;

struct {
    component_t* super;
} frame_s;

bool init(frame_t** self, frame_data_t data);

// view
typedef struct view_s view_t;

typedef struct {
    view_t* next;
} view_data;

typedef struct {
} view_vmt;

struct {
    component_t* super;
    view_data    data;
    view_vmt     vmt;
} view_s;

struct {
} layout_s;

struct {
    void (*on_click)(void);
} bottom_t;

struct {
} scrollbar_t;
=======
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
>>>>>>> model-m-painter
