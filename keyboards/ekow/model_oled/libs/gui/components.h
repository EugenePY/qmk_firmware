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

// frame
typedef struct frame_s frame_t;

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
