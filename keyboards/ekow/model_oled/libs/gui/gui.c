#include "gui.h"
#include "display.h"
#include "timer.h"
#include "debug.h"
#include "oled_main.h"
#include <chthreads.h>
#include <chmtx.h>
#include <stdlib.h>

// APP thread, for event listen, component state update, control frames.
static THD_WORKING_AREA(waAPPEventThread, 256);
static THD_FUNCTION(APPEventThread, arg) {
    app_t* app = arg;
    while (true) {
        chMtxLock(app->display->display_mutex);
        ComponentEntry_t* component_entry;
        LIST_FOREACH(component_entry, app->component_list, entries) {
            COMPONENT_CALL(component_entry->component_pt, event_cb, 1);
        }
        chMtxUnlock(app->display->display_mutex);
        chThdYield();
    }
}
// the frame cannot be change whin the single frame proccess, since the operation will be block the
// task should be as fast as possible
static THD_WORKING_AREA(waAPPRenderingThread, 256);
static THD_FUNCTION(APPRendeThread, arg) {
    (void)arg;
    while (true) {
        chThdYield();
    }
}
/* Hardware Initializtion*/
// MAIN FRAME
// setup components
bool AppInit(app_t** self) {
    if (*self != NULL) return false; // the pointer is already initialized.
    if (NULL == (*self = malloc(sizeof(app_t)))) return false;

    // config threads
    static thread_descriptor_t oled_thread_des = {.name = "oled-render", .wbase = THD_WORKING_AREA_BASE(waAPPRenderingThread), .wend = THD_WORKING_AREA_END(waAPPRenderingThread), .prio = NORMALPRIO, .funcp = APPRendeThread, .arg = NULL};

    static thread_descriptor_t app_thread_des = {.name = "app-thread", .wbase = THD_WORKING_AREA_BASE(waAPPEventThread), .wend = THD_WORKING_AREA_END(waAPPEventThread), .prio = NORMALPRIO, .funcp = APPEventThread, .arg = NULL};

    oled_thread_des.arg = *self;
    app_thread_des.arg  = *self;

    thread_t* oled_thread_tp = chThdCreateSuspended(&oled_thread_des);
    thread_t* app_thread_tp  = chThdCreateSuspended(&app_thread_des);
    if ((oled_thread_tp == NULL) | (app_thread_tp == NULL)) chSysHalt("thread start failed");

    (*self)->render_thread_pt = oled_thread_tp;
    (*self)->app_thread_pt    = app_thread_tp;
    img_init();
    display_init(&(*self)->display);

    // List Initialization
    (*self)->component_list = malloc(sizeof(ComponentList_t));
    LIST_INIT((*self)->component_list);

    // components
    component_t*            component_pt     = NULL;
    static component_data_t component_config = {.x = 0, .y = 0, .width = 64, .height = 48, .active = true};
    ComponentInit((void**)&component_pt, &component_config);
    ComponentEntry_t* entry_pt = malloc(sizeof(ComponentEntry_t));
    entry_pt->component_pt     = (void*)component_pt;
    LIST_INSERT_HEAD((*self)->component_list, entry_pt, entries);
    return true;
}
/*Software/Data Initializtion*/
bool AppStart(app_t* self) {
    display_on(self->display);
    display_render_mode_on(self->display);
    // do not allow to send command during app started, to reduce module error.
    chThdStart(self->app_thread_pt);
    chThdStart(self->render_thread_pt);
    return true;
}

bool AppActive(app_t* self) {
    // thread start
    chThdStart(self->app_thread_pt);
    chThdStart(self->render_thread_pt);
    return true;
}

/*Task yield from the main loop*/
void AppTask(app_t* self) {
    /* Task yield from the main loop*/
    chThdYield();
}

bool AppDeactive(app_t* self) {
    //  thread suspended
    return true;
}

/* free app resource */
bool AppStop(app_t** self) {
    chThdTerminate((*self)->app_thread_pt);
    chThdWait((*self)->app_thread_pt);
    return true;
}
