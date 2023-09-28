#include <stdio.h>
#include <stdint.h>

#include "ssd1331.h"
#include "debug.h"
#include "wait.h"

oled_driver_t oled_driver    = {
    .oled_initialized = false,
    .oled_active = false,
    .oled_dimed = false,
}

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
    wait_ms(5); // wait for the power bump to charge...
};

static void nkk_oled_sw_setup(void) {
    // Setup OLED reset, chargin pump power ping.
    setPinOutput(OLED_SHWN_PIN);
    setPinOutput(OLED_REST_PIN);
    setPinOutput(OLED_SSD_1331_DC_PIN);
}

static void nkk_oled_sw_init(void) {
    nkk_oled_sw_init();
    nkk_oled_sw_reset_on();
}

bool is_oled_driver_actived(void) {
    return oled_driver.oled_active;
}

bool oled_init(oled_rotation_t rotation) {
    // hardware setup
    if (!oled_driver.oled_initialized) {
        nkk_oled_switch_init();
        oled_driver.oled_initialized = true;
    }
    return oled_driver.oled_initialized;
};

bool oled_on(painter_device_t device) {
    if (oled_driver.oled_initialized) {
        if (!oled_driver.oled_active) {
            painter_driver_t *driver    = (painter_driver_t *)device;
            const uint8_t     command[] = {SSD1331_CMD_DISPLAYON};
            if (result == SPI_STATUS_SUCCESS) {
                oled_driver.oled_active = true;
            }
        }
    }
    return oled_driver.oled_active;
}

bool oled_dim(painter_device_t device) {
    if (oled_driver.oled_initialized) {
        if (oled_driver.oled_active & !oled_driver.oled_dimed) {
            // sending the command
            const uint8_t command[] = {SSD1331_CMD_DIMDISPLAY};
            spi_status_t  result    = _command_transaction(command, 1);
        }
    }
    return oled_driver.oled_active;
}

bool oled_off(painter_device_t device) {
    if (oled_driver.oled_initialized) {
        if (oled_driver.oled_active) {
            // sending the command
            const uint8_t command[] = {SSD1331_CMD_DISPLAYOFF};
            spi_status_t  result    = _command_transaction(command, 1);

            if (result == SPI_STATUS_SUCCESS) {
                oled_driver.oled_active = false;
            }
        }
    }
    return oled_driver.oled_active;
}

/* this command it the shutdown percedual suggeset in the datasheet, however
 * in the kb applicaiton user just unplug the usb cable ....
 * */
bool oled_shutdown(painter_device_t device) {
    if (oled_off(device)) {
        writePinLow(OLED_SHWN_PIN);
        return true;
    } else {
        return false;
    }
}
