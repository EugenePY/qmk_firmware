#include <stdbool.h>
#include "quantum.h"
#include "oled_main.h"
#include "usb_util.h"
#include "msd_main.h"
#include "bootloader.h"
//#include "ssd1331.h"

// static const uint8_t img[] = {0x4a, 0x47, 0xc6, 0x12};
// static bool stop_render = true;
//
//
static thread_t *thread;

// Worker thread
// Rendering thread and process entry point.
static THD_WORKING_AREA(waOLEDListenerThread, 128);
static THD_FUNCTION(OLEDListenerThread, arg) {
    (void)arg;
    chRegSetThreadName("OLEDListenerThread");
    while (!chThdShouldTerminateX()) {
        chThdSleepMilliseconds(500);
    }
}

// reset to default image from eeprom
void model_oled_reset_image(void) {}

extern uint32_t __ram0_end__; // bootloader
                              //
#define SYMVAL(sym) (uint32_t)(((uint8_t *)&(sym)) - ((uint8_t *)0))
#define BOOTLOADER_MAGIC 0xDEADBEEF
#define OLED_FLASH_MAGIC 0x32003450

#define MAGIC_ADDR (unsigned long *)(SYMVAL(__ram0_end__) - 4)

void model_oled_flash_img_jump(void) {
    *MAGIC_ADDR = OLED_FLASH_MAGIC;
    __disable_irq(); // disable all interupt
    NVIC_SystemReset();
}

void if_requested_model_oled_flash(void) {
    unsigned long *check = MAGIC_ADDR;
    if (*check == OLED_FLASH_MAGIC) {
        *check = 0;
        msd_protocol_setup();
        while (1) // not goin to the main function
            ;
    }
}

void oled_task_init(void) {
    // It seem that qmk only put data in the heap .... 
    thread = chThdCreateStatic(waOLEDListenerThread, sizeof(waOLEDListenerThread), NORMALPRIO + 2, OLEDListenerThread, NULL);
    if (thread == NULL) chSysHalt("oled-task-thread: out of memory");
}

// OLED task stop
void oled_task_stop(void) {
    chThdTerminate(thread);
    chThdWait(thread);
    thread = NULL;
}

// event trigger task
bool oled_task_user(void) {
    return true;
}
