#include <stdbool.h>
#include "quantum.h"

#include "oled_main.h"
#include "usb_util.h"
#include "msd_main.h"

#include "bootloader.h"
#include "ssd1331.h"
#include "img/icon.h"
#include "stm32_dma.h"

// file system
#include "flash_ioblock.h"
#include "vfat.h"

// static bool stop_render = true;
//
#define RENDER_BUFFER_SIZE IMG_COL * 1 * 2 // image_width, image_height, bytes by rgb565

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

img_t    render_img;
uint8_t *end_addr = NULL;
size_t   img_size = 0;

void oled_task_init(void) {
    oled_init(0);
    img_init();
    get_current_img(&render_img);

    end_addr = (uint8_t *)(render_img.start_addr + render_img.n_frame * IMG_BUFFER_SIZE);
    img_size = render_img.n_frame * IMG_BUFFER_SIZE;
    wait_ms(5);
    oled_on();
    ssd1331_oled_setup_window();

    // thread = chThdCreateStatic(waOLEDListenerThread, sizeof(waOLEDListenerThread), NORMALPRIO +5, OLEDListenerThread, NULL);
    // if (thread == NULL) chSysHalt("oled-task-thread: out of memory");
    //  turn on the oled
}

// OLED task stop
void oled_task_stop(void) {
    // chThdTerminate(thread);
    // chThdWait(thread);
    // thread = NULL;
}

// event trigger task
bool oled_task_user(void) {
    if (is_oled_driver_init()) {
        if (img_size <= 0) {
            ssd1331_oled_setup_window();
            img_size = render_img.n_frame * IMG_BUFFER_SIZE;
        }
        size_t n = MIN(RENDER_BUFFER_SIZE, img_size);
        // ssd1331_oled_render(&img[render_img.n_frame*IMG_BUFFER_SIZE - img_size], n);
        ssd1331_oled_render(end_addr - img_size, n);
        img_size -= n;
        // see nkk sw, page 13.
    }
    return true;
}
