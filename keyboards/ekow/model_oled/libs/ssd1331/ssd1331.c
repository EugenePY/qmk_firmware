#include "ssd1331.h"
#include "printf.h"
#include "debug.h"

#ifndef OLED_TEST
#    include "wait.h"
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

OLED_BUFFER_TYPE oled_buffer[OLED_MATRIX_SIZE];

oled_driver_t oled_driver = {.oled_cursor = &oled_buffer[0], .oled_scrolling = false, .oled_initialized = false, .oled_active = false, .oled_rotation_width = OLED_DISPLAY_WIDTH, .oled_dirty = 0};

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
        spi_stop();
    } else {
        result = SPI_STATUS_ERROR;
    }
    writePinHigh(OLED_SSD_1331_DC_PIN);
    return result;
};

// image data is in uint16_t
spi_status_t STATIC _data_write(const uint8_t* data, uint16_t length) {
    // pull the D/C pin high when sending data.
    writePinHigh(OLED_SSD_1331_DC_PIN);
    wait_us(10);
    spi_status_t status = SPI_STATUS_SUCCESS;
    if (ssd1331_spi_start()) {
        // 65k format, uint16_t, lower byte, and upper byte.
        spi_transmit(data, length);
        spi_stop();
    } else {
        status = SPI_STATUS_ERROR;
    }
    return status;
}
/*
 *  this setup the render windown and reset the pixel cursor to upper left corner.
 */
spi_status_t STATIC _setup_render_window(void) {
    const uint8_t command_buffer[] = {SSD1331_CMD_SETCOLUMN, 0x10, 0x4F, SSD1331_CMD_SETROW, 0x00, 0x2F};
    wait_ms(50);
    return _command_transaction(command_buffer, 6);
}

// oled cursor tracking the current position of the OLED IC.
OLED_BUFFER_TYPE* oled_cursor(void) {
    return oled_driver.oled_cursor;
}

void STATIC nkk_oled_sw_reset_on(void) {
    // OLED_SHWN_PIN should pull down.
    // reset the OLED
    writePinLow(OLED_SHWN_PIN);
    // pull the OLED_RESET_PIN LOW, this pin is low as default.
    writePinLow(OLED_REST_PIN);
    // wait for minumnm of 3us, and then set to high
    wait_ms(100);
    writePinHigh(OLED_REST_PIN);
    wait_ms(50);
    // turn the OLED VCC power on
    writePinHigh(OLED_SHWN_PIN);
};

bool is_oled_driver_actived(void) {
    return oled_driver.oled_active;
}

bool oled_init(oled_rotation_t rotation) {
    // hardware setup
    if (!oled_driver.oled_initialized) {
        spi_init();
        // Setup OLED reset, chargin pump power ping.
        setPinOutput(OLED_SHWN_PIN);
        setPinOutput(OLED_REST_PIN);
        setPinOutput(OLED_SSD_1331_DC_PIN);
        // oled reset Initialization sequence sequence
        nkk_oled_sw_reset_on();
        // Initialization Configuration
        spi_status_t result;

        const uint8_t command_buffer[37] = {SSD1331_CMD_CONTRASTA,
                                            0x15,
                                            SSD1331_CMD_CONTRASTB,
                                            0x1A,
                                            SSD1331_CMD_CONTRASTC,
                                            0x17,
                                            SSD1331_CMD_MASTERCURRENT,
                                            0x0F,
                                            SSD1331_CMD_SETREMAP,
                                            0x70, // segment remap config= 0b01101000, Vertical Address Incremental, RAM col, RGB normal order. 65k color format1.
                                            SSD1331_CMD_STARTLINE,
                                            0x00,
                                            SSD1331_CMD_DISPLAYOFFSET,
                                            0x10,
                                            SSD1331_CMD_NORMALDISPLAY,
                                            SSD1331_CMD_SETMULTIPLEX,
                                            0x2F,
                                            SSD1331_CMD_DIMMODESETTING,
                                            0x00,
                                            0x12,
                                            0x0C,
                                            0x14,
                                            0x12,
                                            SSD1331_CMD_SETMASTER,
                                            0x8E,
                                            SSD1331_CMD_POWERMODE,
                                            0x0B, // Power save mode
                                            SSD1331_CMD_PRECHARGE,
                                            0x44,
                                            SSD1331_CMD_CLOCKDIV,
                                            0xA0,
                                            SSD1331_CMD_ENABLELINEARGRAYSCALE,
                                            SSD1331_CMD_PRECHARGELEVEL,
                                            0x12,
                                            SSD1331_CMD_VCOMH,
                                            0x3E,
                                            SSD1331_CMD_DISPLAYON}; // turn the display off by default.
        result                           = _command_transaction(command_buffer, 37);
        wait_ms(500);
        ssd1331_oled_setup_window();

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

// get the correpoding color of given buffer index.
bool ssd1331_oled_write_window(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    x = MAX(MIN(x, OLED_DISPLAY_WIDTH), 0x00);
    y = MAX(MIN(y, OLED_DISPLAY_HEIGHT), 0x00);
    w = MAX(w - 1, 0x00);
    h = MAX(w - 1, 0x00);
    w = MIN(w, OLED_DISPLAY_WIDTH - w);
    h = MIN(h, OLED_DISPLAY_HEIGHT - h);

    spi_status_t result = SPI_STATUS_SUCCESS;
    // sending the command
    const uint8_t command_buffer[6] = {SSD1331_CMD_SETCOLUMN, 0x0, w, SSD1331_CMD_SETROW, 0x0, h};
    result                          = _command_transaction(command_buffer, 6);

    // col based indexing
    for (int r = y; r < h; r++) {
        const OLED_BUFFER_TYPE* block_buffer = &oled_buffer[OLED_DISPLAY_WIDTH * r + x];
        result                               = _data_write(block_buffer, w);
    }
    return result == SPI_STATUS_SUCCESS;
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
