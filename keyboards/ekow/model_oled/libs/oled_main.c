#include <stdbool.h>
#include "quantum.h"
#include "oled_main.h"
#include "usb_util.h"
#include "msd_main.h"
#include "bootloader.h"
#include "ssd1331.h"
#include "img/icon.h"
#include "stm32_dma.h"
// static bool stop_render = true;
//
#define RENDER_BUFFER_SIZE IMG_COL * 1 * 2 // image_width, image_height, bytes by rgb565

static thread_t *thread;
static uint32_t  idx = 0;

// TODO: using DMA for double buffering
uint8_t rendering_buffer[RENDER_BUFFER_SIZE];
// Worker thread
// Rendering thread and process entry point.
static THD_WORKING_AREA(waOLEDListenerThread, 256);
static THD_FUNCTION(OLEDListenerThread, arg) {
    (void)arg;
    chRegSetThreadName("OLEDListenerThread");
    while (!chThdShouldTerminateX()) {
        while (is_oled_driver_init()) {
            if (idx >= IMG_BUFFER_SIZE) {
                idx = 0;
                ssd1331_oled_setup_window();
            }
            // see nkk sw, page 13.
            for (uint16_t i = 0; i < RENDER_BUFFER_SIZE; i++) {
                rendering_buffer[i] = img[idx];
                idx += 1;
            }
            ssd1331_oled_render(rendering_buffer, RENDER_BUFFER_SIZE);
        }
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
    oled_init(0);
    // It seem that qmk only put data in the heap ....
    while (!is_oled_driver_init()) {
        wait_ms(50); // wait for oled to iniit
    }
    thread = chThdCreateStatic(waOLEDListenerThread, sizeof(waOLEDListenerThread), NORMALPRIO + 2, OLEDListenerThread, NULL);
    if (thread == NULL) chSysHalt("oled-task-thread: out of memory");
    // turn on the oled
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
