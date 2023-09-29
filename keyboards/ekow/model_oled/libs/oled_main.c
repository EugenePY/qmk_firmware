#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "quantum.h"
#include "msd_main.h"
#include "bootloader.h"

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

USBMassStorageDriver UMSD1;

bool if_requested_model_oled_flash(void) {
    unsigned long *check = MAGIC_ADDR;
    if (*check == OLED_FLASH_MAGIC) {
        *check = 0;
        msd_protocol_setup(&UMSD1);
        event_listener_t USBEjectListener;
        chEvtRegisterMask(&UMSD1.evt_ejected, &USBEjectListener, EVENT_MASK(0));

        while (true) {
            eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
            if (evt & EVENT_MASK(0)) {
                msdStop(&UMSD1);
                NVIC_SystemReset();
            }
        }
        return true;
    }
    return false;
}
