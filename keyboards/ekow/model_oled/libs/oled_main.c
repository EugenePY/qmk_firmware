#include <stdbool.h>
#include <stdint.h>
#include <string.h>
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
#include "wait.h"

// static bool stop_render = true;
//
#define RENDER_BUFFER_SIZE (IMG_COL * 1 * 2) * 4 // image_width, image_height, bytes by rgb565

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

image_file_t  image_file;
image_file_t *image_file_pt = (void *)NULL;
const char    HEADER[]      = "<OLED.IMG>";

static void load_defualt_image(void) {
    image_file_t default_image_file = {.header = {.n_frame = 1, .time_delay = 0}, .img_buffer = (uint8_t *)&img};
    image_file                      = default_image_file;
    image_file_pt                   = &image_file;
}

/* If the image is not empty, we load image from
 */
static void load_image(void) {
    uint8_t      offset          = strlen(HEADER);
    image_file_t load_image_file = {.header     = *((image_header_t *)FLASH_ADDR(offset)), // load from flash
                                    .img_buffer = (uint8_t *)FLASH_ADDR(offset + sizeof(image_header_t))};
    image_file                   = load_image_file;
    image_file_pt                = &image_file;
}

static bool check_image_is_empty(void) {
    uint8_t input_block_buffer[SECTOR_SIZE_BYTES];
    // header present in the first block of the flash
    vfs_read_fat12(DISK_BLOCK_DataStartBlock, input_block_buffer);
    bool is_empty = false;
    for (int i = 0; i < strlen(HEADER); i++) {
        if (HEADER[i] != input_block_buffer[i]) {
            is_empty = true;
        }
    }
    return is_empty;
}

void img_init(void) {
    if (check_image_is_empty()) {
        load_defualt_image();
    } else {
        load_image();
    }
}

//
static int giframe_unsend_byte = 0;            // byte left of current frame.
static int giframe             = 0;            // index of the frame
uint8_t    color[2]            = {0xff, 0xff}; //
static THD_WORKING_AREA(waOLEDThread, 256);
static THD_FUNCTION(OLEDThread, arg) {
    while (true) {
        if ((giframe_unsend_byte > 0)) {
            int n = 2; // MIN(RENDER_BUFFER_SIZE, giframe_unsend_byte);
            // ssd1331_oled_render(image_file_pt->img_buffer + (giframe)*OLED_BUFFER_SIZE - giframe_unsend_byte, n);
            ssd1331_oled_render(color, n);
            giframe_unsend_byte -= n;
        } else {
            giframe_unsend_byte = OLED_BUFFER_SIZE;

            if ((giframe + 1) > image_file_pt->header.n_frame) {
                giframe = 1;
            } else {
                giframe += 1;
            }

            if (image_file_pt->header.n_frame > 1) {
                chThdSleepMilliseconds((const int)image_file_pt->header.time_delay);
            } else {
                chThdSleepMilliseconds(1000); // image render with slower rerender.
            }
        }
        chThdYield();
    }
}

/* Load data, if image is empty we put default image to the
 */
thread_t *thread = NULL;

void oled_task_init(void) {
    img_init();
    if (is_oled_driver_init()) {
        oled_on();
        ssd1331_oled_setup_window();
        thread = chThdCreateStatic(waOLEDThread, sizeof(waOLEDThread), NORMALPRIO, OLEDThread, NULL);
    }
    //  turn on the oled
}

// OLED task stop
void oled_task_stop(void) {
    chThdTerminate(thread);
    chThdWait(thread);
    thread = NULL;
}

bool oled_task_user(void) {
    // see nkk sw, page 13.
    // oled_frame_loop();
    chThdYield();
    return true;
}
