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
