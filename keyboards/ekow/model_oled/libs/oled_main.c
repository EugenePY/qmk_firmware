#include "quantum.h"
#include "oled_main.h"
#include "usb_util.h"
#include "msd_main.h"
#include "ssd1331.h"

// static const uint8_t img[] = {0x4a, 0x47, 0xc6, 0x12};
static bool stop_render = true;

event_source_t oled_event_source;
thread_t      *thread;

void model_oled_as_msd(void) {
    usb_disconnect();
    msd_protocol_setup(); // msd msd_protocol_entry point.
}

// event hander and process entry point.
static THD_WORKING_AREA(waOLEDListenerThread, 128);
static THD_FUNCTION(OLEDListenerThread, arg) {
    (void)arg;

    chRegSetThreadName("OLEDListenerThread");
    // Event Registeration
    event_listener_t task_listener;
    chEvtRegisterMask(&oled_event_source, &task_listener, FLASHING_EVT);
    chEvtRegisterMask(&oled_event_source, &task_listener, STOP_RENDER_EVT);

    while (!chThdShouldTerminateX()) {
        eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
        switch (evt) {
            case (FLASHING_EVT):
                model_oled_as_msd();
            case (STOP_RENDER_EVT):
                stop_render = true;
                // ssd1331_render_stop(driver);
            default:
                break;
        }
    }
}

// reset to default image from eeprom
void model_oled_reset_image(void) {}

void model_oled_enject(void) {
    bootloader_jump();
}

void oled_task_init(void) {
    chEvtObjectInit(&oled_event_source);
    thread = chThdCreateStatic(waOLEDListenerThread, sizeof(waOLEDListenerThread), NORMALPRIO - 16, OLEDListenerThread, NULL);
}

// OLED task stop
void oled_task_stop(void) {
    chThdTerminate(thread);
    thread = NULL;
};

// event trigger task
bool oled_task_user(void) {
    return true;
}

// event dispatcher from qmk
