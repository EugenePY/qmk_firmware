<<<<<<< HEAD
#pragma once
#include <stdint.h>
#include <ch.h>

#define WIDTH_TYPE uint8_t
#define HEIGHT_TYPE uint8_t
#define RENDER_BUFFER_SIZE 256

#define METHOD(obj, method)
=======
#ifndef _GUI_H_
#define _GUI_H_
#include "components.h"
#include "display.h"
#include <stdint.h>
#include <ch.h>

>>>>>>> model-m-painter

#define TRANS_MASK 0xff
// colors

<<<<<<< HEAD
#define PIXEL_TYPE uint16_t

uint8_t getR(PIXEL_TYPE pixel);
uint8_t getG(PIXEL_TYPE piexl);
uint8_t getB(PIXEL_TYPE pixel);

//
typedef struct app_s app_t;
typedef struct event_s event_t;

struct {
    uint16_t event_code;
    uint16_t event_data;
} event_s;

// component interface
//

// screen_driver

typedef struct {
} display_driver_t;

// App
struct {
    display_driver_t* device_pt;     // display driver
    thread_t*         msg_thread_pt; //  main messaging thread
    view_t*           root_view_pt;  // ROOT view
} app_s;
=======
// screen_driver

// TODO: make the pointer as the union type for more safe usage.
typedef struct ComponentEntry {
    void* component_pt; // using void pointer this should only put component_t.
    LIST_ENTRY(ComponentEntry) entries;
} ComponentEntry_t;

typedef LIST_HEAD(ComponentList, ComponentEntry) ComponentList_t;

typedef struct app_s {
    display_driver_t* display;
    thread_t*         app_thread_pt; //  main messaging thread
    thread_t*         render_thread_pt;
    ComponentList_t*  component_list; // first component for rendering.
} app_t;

bool AppInit(app_t** self);
bool AppStart(app_t* self);
void AppTask(app_t* self);
bool AppStop(app_t** self);
#endif
>>>>>>> model-m-painter
