#ifndef _GUI_H_
#define _GUI_H_
#include "components.h"
#include "display.h"
#include <stdint.h>
#include <ch.h>


#define TRANS_MASK 0xff
// colors

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
