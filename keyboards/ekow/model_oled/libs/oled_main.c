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
#include "ff.h"
#include "diskio.h"
#include "flash_ioblock.h"

// static bool stop_render = true;
//
#define RENDER_BUFFER_SIZE IMG_COL * 1 * 2 // image_width, image_height, bytes by rgb565

static thread_t *thread;
static uint32_t  idx = 0;
uint8_t          rendering_buffer[RENDER_BUFFER_SIZE];
// TODO: using DMA for double buffering

FATFS               fs;                            /* Filesystem object */
FIL                 render_file;                   /* File object */
const static TCHAR *target_file_name = "OLED.IMG"; // default file name
BYTE                work[FF_MAX_SS];

FILINFO fno;

// Worker thread
// Rendering thread and process entry point.
static THD_WORKING_AREA(waOLEDListenerThread, 1024);
static THD_FUNCTION(OLEDListenerThread, arg) {
    (void)arg;
    chRegSetThreadName("OLEDListenerThread");
    FRESULT res;
    res = f_open(&render_file, target_file_name, FA_READ);
    if (res != FR_OK) chSysHalt("open file for reading failed");

    while (!chThdShouldTerminateX()) {
        while (is_oled_driver_init()) {
            if (idx >= IMG_BUFFER_SIZE) {
                idx = 0;
                // ssd1331_oled_setup_window();
            }
            // see nkk sw, page 13.
            UINT    br;
            FRESULT res = f_read(&render_file, rendering_buffer, RENDER_BUFFER_SIZE, &br);
            if (res != FR_OK) chSysHalt("read file failed");
            idx += br;
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

void create_default_file(void) {
    FRESULT res = f_open(&render_file, target_file_name, FA_CREATE_NEW | FA_WRITE);
    if (res != FR_OK) chSysHalt("file create failed");
    UINT bw;
    res = f_write(&render_file, "hello, world\n", 13, &bw);
    if (bw < 13) chSysHalt("volume full");
    if (res != FR_OK) chSysHalt("file write failed");
    res = f_close(&render_file);
    if (res != FR_OK) chSysHalt("file close failed");
}

void oled_task_init(void) {
    /*
    BYTE buffer[512];
    memset(buffer, 0x11, 512);
    disk_initialize(0);
    disk_write(0, buffer, 67, 1);
    BYTE buffer_read[512];
    disk_read(0, buffer_read, 67, 1);
    bool res_test = false;
    for (int i =0; i < 512; i++) {
        res_test = buffer_read[i] == buffer[i];
        res_test = buffer[i] == 0x11;
        if (!res_test) break;
    }
    if (!res_test) chSysHalt("wrong");
    chSysHalt("halt for testing");
    */

    FRESULT res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        switch (res) {
            case FR_NO_FILESYSTEM:
                res = f_mkfs("", 0, work, sizeof(work));
                if (res != FR_OK) chSysHalt("mkfs failed");
                break;
            default:
                chSysHalt("oled_task_init: f mounting failed");
        }
    }
    res = f_stat(target_file_name, &fno);
    if (res == FR_NO_FILE) create_default_file();

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
