#include <stdio.h>
#include <stdint.h>

#include "ssd1331.h"
#include "debug.h"
#include "wait.h"

#ifndef OLED_TEST
#    include "gpio.h"
#    include "spi_master.h"
#else
#    include "ssd1331_mock.h"
#endif
// Some helper function

#define OLED_ALL_BLOCKS_MASK (((((OLED_BLOCK_TYPE)1 << (OLED_BLOCK_COUNT - 1)) - 1) << 1) | 1)

// get the lower 1 byte of 2 byte data
#define _8BITMASK(bit) (uint16_t)(bit) & 0xff00
#define SHIFT_1_BYTE(data) (uint16_t) data >> 8

// Write RGB color
#define RGB565_BITMASK_R(r) (uint8_t) r << 3
#define RGB565_BITMASK_G(g) (uint8_t) g << 2
#define RGB565_BITMASK_B(b) (uint8_t) b << 3

#define RGB565_BITMASK_R_16bit(r) (uint16_t) RGB565_BITMASK_R(r)
#define RGB565_BITMASK_G_16bit(g) ((uint16_t)RGB565_BITMASK_G(g)) << 5
#define RGB565_BITMASK_B_16bit(b) ((uint16_t)RGB565_BITMASK_B(b)) << 11

#define RGB565(r, g, b) RGB565_BITMASK_R_16bit(r) | RGB565_BITMASK_G_16bit(g) | RGB565_BITMASK_B_16bit(b)

uint8_t       oled_buffer[1] = {0};
oled_driver_t oled_driver    = {.oled_cursor = &oled_buffer[0], .oled_scrolling = false, .oled_initialized = false, .oled_active = false, .oled_rotation_width = OLED_DISPLAY_WIDTH, .oled_dirty = 0};

/*
 * Do not initial the MISO
 *
 */
#ifndef OLED_TEST

void spi_init(void) {
    static bool is_initialised = false;
    if (!is_initialised) {
        is_initialised = true;
        // Try releasing special pins for a short time
        setPinInput(SPI_SCK_PIN);

        setPinInput(SPI_MOSI_PIN);

        chThdSleepMilliseconds(10);
#    if defined(USE_GPIOV1)
        palSetPadMode(PAL_PORT(SPI_SCK_PIN), PAL_PAD(SPI_SCK_PIN), SPI_SCK_PAL_MODE);

        palSetPadMode(PAL_PORT(SPI_MOSI_PIN), PAL_PAD(SPI_MOSI_PIN), SPI_MOSI_PAL_MODE);

#    else
        palSetPadMode(PAL_PORT(SPI_SCK_PIN), PAL_PAD(SPI_SCK_PIN), PAL_MODE_ALTERNATE(SPI_SCK_PAL_MODE) | PAL_OUTPUT_TYPE_PUSHPULL | PAL_OUTPUT_SPEED_HIGHEST);
        palSetPadMode(PAL_PORT(SPI_MOSI_PIN), PAL_PAD(SPI_MOSI_PIN), PAL_MODE_ALTERNATE(SPI_MOSI_PAL_MODE) | PAL_OUTPUT_TYPE_PUSHPULL | PAL_OUTPUT_SPEED_HIGHEST);
#    endif
    }
}
#endif

bool STATIC ssd1331_spi_start(void) {
    return spi_start(OLED_SS_PIN, false, 0, OLED_SPI_CLK_DIVISOR);
}
// hardware command
spi_status_t STATIC _command_transaction(const uint8_t* command_data, uint16_t lenght) {
    writePinLow(OLED_SSD_1331_DC_PIN);
    spi_status_t result = SPI_STATUS_SUCCESS;
    if (ssd1331_spi_start()) {
        result = spi_transmit(command_data, lenght);
        writePinHigh(OLED_SSD_1331_DC_PIN);
        spi_stop();
    } else {
        result = SPI_STATUS_ERROR;
    }
    writePinHigh(OLED_SSD_1331_DC_PIN);
    spi_stop();
    return result;
};

// image data is in uint16_t
spi_status_t STATIC _data_write(const uint8_t* data, uint16_t length) {
    // pull the D/C pin high when sending data.
    writePinHigh(OLED_SSD_1331_DC_PIN);
    spi_status_t status = SPI_STATUS_SUCCESS;
    if (ssd1331_spi_start()) {
        // 65k format, uint16_t, lower byte, and upper byte.
        spi_transmit(data, length);
        spi_stop();
    } else {
        status = SPI_STATUS_ERROR;
    }
    spi_stop();
    return status;
}
/*
 *  this setup the render windown and reset the pixel cursor to upper left corner.
 */
spi_status_t STATIC _setup_render_window(void) {
    spi_status_t  res;
    const uint8_t _command_buffer[] = {SSD1331_CMD_SETCOLUMN, 0x10, 0x4F};
    res                             = _command_transaction(_command_buffer, 3);
    if (res != SPI_STATUS_SUCCESS) return res;
    wait_ms(5);
    const uint8_t command_buffer[] = {SSD1331_CMD_SETROW, 0x00, 0x2F};
    res                            = _command_transaction(command_buffer, 3);
    wait_ms(5);
    return res;
}

// oled cursor tracking the current position of the OLED IC.
uint8_t* oled_cursor(void) {
    return oled_driver.oled_cursor;
}

void STATIC nkk_oled_sw_reset_on(void) {
    // OLED_SHWN_PIN should pull down.
    // reset the OLED
    writePinLow(OLED_SHWN_PIN);
    wait_ms(20);
    // pull the OLED_RESET_PIN LOW, this pin is low as default.
    writePinLow(OLED_REST_PIN);
    // wait for minumnm of 3us, and then set to high
    wait_ms(20);
    writePinHigh(OLED_REST_PIN);
    wait_ms(20);
    // turn the OLED VCC power on
    writePinHigh(OLED_SHWN_PIN);
    wait_ms(200); // wait for the power bump to charge...
};

bool is_oled_driver_actived(void) {
    return oled_driver.oled_active;
}

bool oled_init(oled_rotation_t rotation) {
    // hardware setup
    if (!oled_driver.oled_initialized) {
        spi_init();
        wait_ms(10);
        // Setup OLED reset, chargin pump power ping.
        setPinOutput(OLED_SHWN_PIN);
        setPinOutput(OLED_REST_PIN);
        setPinOutput(OLED_SSD_1331_DC_PIN);
        // oled reset Initialization sequence sequence
        nkk_oled_sw_reset_on();
        // Initialization Configuration
        spi_status_t result;
        // OPCode, Dely, n data
        uint8_t initialize_seq[] = {SSD1331_CMD_CONTRASTA,
                                    5,
                                    1,
                                    0x15,
                                    SSD1331_CMD_CONTRASTB,
                                    5,
                                    1,
                                    0x1A,
                                    SSD1331_CMD_CONTRASTC,
                                    5,
                                    1,
                                    0x17,
                                    SSD1331_CMD_MASTERCURRENT,
                                    5,
                                    1,
                                    0x0F,
                                    SSD1331_CMD_SETREMAP,
                                    5,
                                    1,
                                    0x70, // segment remap config= 0b01101000, Vertical Address Incremental, RAM col, RGB normal order. 65k color format1.
                                    SSD1331_CMD_STARTLINE,
                                    5,
                                    1,
                                    0x00,
                                    SSD1331_CMD_DISPLAYOFFSET,
                                    5,
                                    1,
                                    0x10,
                                    SSD1331_CMD_NORMALDISPLAY,
                                    5,
                                    0,
                                    SSD1331_CMD_SETMULTIPLEX,
                                    5,
                                    1,
                                    0x2F,
                                    SSD1331_CMD_DIMMODESETTING,
                                    5,
                                    5,
                                    0x00,
                                    0x12,
                                    0x0C,
                                    0x14,
                                    0x12,
                                    SSD1331_CMD_SETMASTER,
                                    5,
                                    1,
                                    0x8E,
                                    SSD1331_CMD_POWERMODE,
                                    5,
                                    1,
                                    0x0B, // Power save mode
                                    SSD1331_CMD_PRECHARGE,
                                    5,
                                    1,
                                    0x44,
                                    SSD1331_CMD_CLOCKDIV,
                                    5,
                                    1,
                                    0xA0,
                                    SSD1331_CMD_ENABLELINEARGRAYSCALE,
                                    5,
                                    0,
                                    SSD1331_CMD_PRECHARGELEVEL,
                                    5,
                                    1,
                                    0x12,
                                    SSD1331_CMD_VCOMH,
                                    5,
                                    1,
                                    0x3E,
                                    SSD1331_CMD_DISPLAYOFF,
                                    5,
                                    0};
        // Total 18 cmd
        uint8_t j = 0;
        for (uint8_t i = 0; i < 18; i++) {
            uint8_t size  = initialize_seq[j + 2] + 1;
            uint8_t delay = initialize_seq[j + 1];
            uint8_t buffer[size];
            buffer[0] = initialize_seq[j];
            if (size > 1) {
                memcpy(&buffer[1], &initialize_seq[j + 3], size);
            }
            result = _command_transaction(buffer, size);
            wait_ms(delay);
            j += size + 2;
        }
        wait_ms(10);

        if (result == SPI_STATUS_SUCCESS) {
            oled_driver.oled_initialized = true;
        } else {
            dprintf("oled_init() failed");
            oled_driver.oled_initialized = false;
        }
    }
    return oled_driver.oled_initialized;
};
bool ssd1331_oled_setup_window(void) {
    return _setup_render_window() == SPI_STATUS_SUCCESS;
}

bool is_oled_driver_init(void) {
    return oled_driver.oled_initialized;
}

bool oled_on(void) {
    if (oled_driver.oled_initialized) {
        if (!oled_driver.oled_active) {
            const uint8_t command[] = {SSD1331_CMD_DISPLAYON};
            spi_status_t  result    = _command_transaction(command, 1);
            if (result == SPI_STATUS_SUCCESS) {
                oled_driver.oled_active = true;
            }
        }
    }
    return oled_driver.oled_active;
}
bool oled_off(void) {
    if (oled_driver.oled_initialized) {
        if (oled_driver.oled_active) {
            // sending the command
            const uint8_t command[] = {SSD1331_CMD_DIMDISPLAY};
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
bool oled_shutdown(void) {
    if (oled_off()) {
        writePinLow(OLED_SHWN_PIN);
        return true;
    } else {
        return false;
    }
}

void ssd1331_oled_render(const uint8_t* img, uint16_t length) {
    _data_write(img, length);
}

void oled_write_rgb_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    x = MIN(x, OLED_DISPLAY_WIDTH);
    y = MIN(y, OLED_DISPLAY_HEIGHT);

    if (x >= oled_driver.oled_rotation_width) {
        return;
    }
    uint16_t index = x + (y / 8) * oled_driver.oled_rotation_width;
    if (index >= OLED_MATRIX_SIZE) {
        return;
    }
    OLED_BUFFER_TYPE data = oled_buffer[index];
    data |= RGB565(r, g, b);
    if (oled_buffer[index] != data) {
        oled_buffer[index] = data;
        oled_driver.oled_dirty |= ((OLED_BLOCK_TYPE)1 << (index / OLED_BLOCK_SIZE));
    }
}
// writing the byte to oled_buffer, this will not change the oled_cursor.
void ssd1331_oled_write_raw_byte(const OLED_BUFFER_TYPE* data, uint16_t index) {
    uint16_t end = sizeof(*data) / sizeof(OLED_BUFFER_TYPE);
    for (uint16_t i = index; i < end; i++) {
        uint16_t idx = i;
        if (i > OLED_MATRIX_SIZE) {
            idx = idx % OLED_MATRIX_SIZE;
        }
        OLED_BUFFER_TYPE data = oled_buffer[idx];
        if (oled_buffer[idx] != data) {
            oled_buffer[idx] = data;
            oled_driver.oled_dirty |= ((OLED_BLOCK_TYPE)1 << (idx / OLED_BLOCK_SIZE));
        }
        data++;
    }
}

void oled_task(void) {
    if (!oled_driver.oled_initialized) {
        return;
    }
#if OLED_UPDATE_INTERVAL > 0
    if (timer_elapsed(oled_update_timeout) >= OLED_UPDATE_INTERVAL) {
        oled_update_timeout = timer_read();
        oled_task_kb();
    }
#else
    oled_task_kb();
#endif
    // Smart render system, no need to check for dirty
    oled_task_user();
}

__attribute__((weak)) bool oled_task_kb(void) {
    return oled_task_user();
}
__attribute__((weak)) bool oled_task_user(void) {
    return true;
}
