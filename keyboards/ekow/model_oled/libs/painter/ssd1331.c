#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ssd1331.h"
#include "debug.h"
#include "wait.h"
#include "gpio.h"
#include "qp_comms.h"
#include "qp_ssd1331_opcodes.h"
#include "color.h"

static oled_driver_t oled_driver = {
    .oled_initialized = false,
    .oled_active      = false,
    .oled_dimed       = false,
    .device_pt        = NULL,
};

static void nkk_oled_sw_reset_on(void) {
    // OLED_SHWN_PIN should pull down.
    // reset the OLED
    wait_ms(5);
    // pull the OLED_RESET_PIN LOW, this pin is low as default.
    writePinLow(OLED_REST_PIN);
    // wait for minumnm of 3us, and then set to high
    wait_ms(5);
    writePinHigh(OLED_REST_PIN);
    wait_ms(5);
    // turn the OLED VCC power on
    writePinHigh(OLED_SHWN_PIN);
    wait_ms(100); // wait for the power bump to charge...
};

static void nkk_oled_sw_setup(void) {
    // Setup OLED reset, chargin pump power ping.
    setPinOutput(OLED_SHWN_PIN);
    setPinOutput(OLED_REST_PIN);
    setPinOutput(OLED_SSD_1331_DC_PIN);
}

static void nkk_oled_switch_init(void) {
    nkk_oled_sw_setup();
    nkk_oled_sw_reset_on();
}

bool is_oled_driver_actived(void) {
    return oled_driver.oled_active;
}

bool oled_init(painter_device_t *device_pt) {
    // hardware setup
    if (!oled_driver.oled_initialized) {
        oled_driver.device_pt = device_pt;
        nkk_oled_switch_init();
        oled_driver.oled_initialized = oled_driver.device_pt != NULL;
    }
    return oled_driver.oled_initialized;
};

bool oled_on(void) {
    if (oled_driver.oled_initialized) {
        if (!oled_driver.oled_active) {
            qp_power(*(oled_driver.device_pt), true);
            oled_driver.oled_active = true;
            oled_driver.oled_dimed  = false;
        }
    }
    return oled_driver.oled_active;
}

void oled_clear(void) {
    if (oled_driver.oled_initialized) {
        if (!oled_driver.oled_active) {
            qp_rect(*(oled_driver.device_pt), 0, 0, OLED_WIDTH - 1, OLED_HEIGHT - 1, HSV_BLACK, true);
        }
    }
}

bool oled_toggle(void) {
    if (oled_driver.oled_initialized) {
        return !oled_off();
    } else {
        return oled_on();
    }
}

bool oled_dim(void) {
    if (oled_driver.oled_initialized) {
        if (oled_driver.oled_active & !oled_driver.oled_dimed) {
            // sending the command
            qp_comms_command(*(oled_driver.device_pt), SSD1331_CMD_DIMDISPLAY);
            oled_driver.oled_dimed = true;
        }
    }
    return oled_driver.oled_dimed;
}

bool oled_off(void) {
    if (oled_driver.oled_initialized) {
        if (oled_driver.oled_active) {
            // sending the command
            qp_power(*(oled_driver.device_pt), false);
            oled_driver.oled_active = false;
        }
    }
    return !oled_driver.oled_active;
}

/* this command it the shutdown percedual suggeset in the datasheet, however
 * in the kb applicaiton user just unplug the usb cable ....
 * */
bool oled_shutdown(void) {
    if (oled_off()) {
        writePinLow(OLED_SHWN_PIN);
        return true;
    } else {
        return false;
    }
}
