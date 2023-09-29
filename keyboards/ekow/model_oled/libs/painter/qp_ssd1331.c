#include <wait.h>
#include "qp_comms.h"
#include "qp_ssd1331.h"
#include "qp_ssd1331_opcodes.h"
#include "qp_tft_panel.h"

#ifdef QUANTUM_PAINTER_SSD1331_SPI_ENABLE
#    include "qp_comms_spi.h"
#endif // QUANTUM_PAINTER_SSD1331_SPI_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common

// Driver storage
tft_panel_dc_reset_painter_device_t ssd1331_drivers[SSD1331_NUM_DEVICES] = {0};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization

bool qp_ssd1331_init(painter_device_t device, painter_rotation_t rotation) {
    tft_panel_dc_reset_painter_device_t *driver = (tft_panel_dc_reset_painter_device_t *)device;

    // clang-format off
    const uint8_t ssd1331_init_sequence[] = {
        // Command,                 Delay, N, Data[N]
        SSD1331_CMD_CONTRASTA,         5,  1, 0x15,
        SSD1331_CMD_CONTRASTB,         5,  1, 0x1A,
        SSD1331_CMD_CONTRASTC,         5,  1, 0x17,
        SSD1331_CMD_MASTERCURRENT,     5,  1, 0x0F,
        SSD1331_CMD_SETREMAP,          5,  1, 0x74, //
        SSD1331_CMD_STARTLINE,         5,  1, 0x00,
        SSD1331_CMD_DISPLAYOFFSET,     5,  1, 0x10,
        SSD1331_CMD_NORMALDISPLAY,     5,  0,
        SSD1331_CMD_SETMULTIPLEX,      5,  1, 0x2F,
        SSD1331_CMD_DIMMODESETTING,    5,  5, 0x00, 0x12, 0x0C, 0x14, 0x12,
        SSD1331_CMD_SETMASTER,         5,  1, 0x8E,
        SSD1331_CMD_POWERMODE,         5,  1, 0x0B,
        SSD1331_CMD_PRECHARGE,         5,  1, 0x44,
        SSD1331_CMD_CLOCKDIV,          5,  1, 0xA0,
        SSD1331_CMD_ENABLELINEARGRAYSCALE, 5, 0,
        SSD1331_CMD_PRECHARGELEVEL,    5,  1, 0x12,
        SSD1331_CMD_VCOMH,             5, 1, 0x3E,
        SSD1331_CMD_DISPLAYOFF,        5, 0
        //SSD1331_DISPLAYON,           5,  0, not turning on the oled when init
    };
    qp_comms_bulk_command_sequence(device, ssd1331_init_sequence, sizeof(ssd1331_init_sequence));

    // Configure the rotation (i.e. the ordering and direction of memory writes in GRAM)
    /*
     const uint8_t madctl[] = {
        [QP_ROTATION_0]   = SSD1331_MADCTL_RGB | SSD1331_MADCTL_MY,
        [QP_ROTATION_90]  = SSD1331_MADCTL_RGB | SSD1331_MADCTL_MX | SSD1331_MADCTL_MY | SSD1331_MADCTL_MV,
        [QP_ROTATION_180] = SSD1331_MADCTL_BGR | SSD1331_MADCTL_MX,
        [QP_ROTATION_270] = SSD1331_MADCTL_BGR | SSD1331_MADCTL_MV,
    };
    */
    //qp_comms_command_databyte(device, SSD1331_CMD_SETREMAP, madctl[rotation]);
    qp_comms_command_databyte(device, SSD1331_CMD_STARTLINE, (rotation == QP_ROTATION_0 || rotation == QP_ROTATION_90) ? 0: driver->base.panel_height);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Driver vtable
bool qp_ssd1331_panel_viewport(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) {
    struct painter_driver_t                           *driver = (struct painter_driver_t *)device;
    struct tft_panel_dc_reset_painter_driver_vtable_t *vtable = (struct tft_panel_dc_reset_painter_driver_vtable_t *)driver->driver_vtable;

    // Fix up the drawing location if required
    left += driver->offset_x;
    right += driver->offset_x;
    top += driver->offset_y;
    bottom += driver->offset_y;

    // Check if we need to manually swap the window coordinates based on whether or not we're in a sideways rotation
    if (vtable->swap_window_coords && (driver->rotation == QP_ROTATION_90 || driver->rotation == QP_ROTATION_270)) {
        uint16_t temp;

        temp = left;
        left = top;
        top  = temp;

        temp   = right;
        right  = bottom;
        bottom = temp;
    }

    if (vtable->num_window_bytes == 1) {
        // Set up the x-window
        uint8_t xbuf[2] = {left & 0xFF, right & 0xFF};
        qp_comms_command_databuf(device, vtable->opcodes.set_column_address, xbuf, sizeof(xbuf));

        // Set up the y-window
        uint8_t ybuf[2] = {top & 0xFF, bottom & 0xFF};
        qp_comms_command_databuf(device, vtable->opcodes.set_row_address, ybuf, sizeof(ybuf));
    } else if (vtable->num_window_bytes == 2) {
        // Set up the x-window
        uint8_t xbuf[4] = {left >> 8, left & 0xFF, right >> 8, right & 0xFF};
        qp_comms_command_databuf(device, vtable->opcodes.set_column_address, xbuf, sizeof(xbuf));

        // Set up the y-window
        uint8_t ybuf[4] = {top >> 8, top & 0xFF, bottom >> 8, bottom & 0xFF};
        qp_comms_command_databuf(device, vtable->opcodes.set_row_address, ybuf, sizeof(ybuf));
    }

    // Lock in the window, ssd1331 donot require to lock the window.
    // qp_comms_command(device, vtable->opcodes.enable_writes);
    return true;
}

const struct tft_panel_dc_reset_painter_driver_vtable_t ssd1331_driver_vtable = {
    .base =
        {
            .init            = qp_ssd1331_init,
            .power           = qp_tft_panel_power,
            .clear           = qp_tft_panel_clear,
            .flush           = qp_tft_panel_flush,
            .pixdata         = qp_tft_panel_pixdata,
            .viewport        = qp_ssd1331_panel_viewport,
            .palette_convert = qp_tft_panel_palette_convert_rgb565_swapped,
            .append_pixels   = qp_tft_panel_append_pixels_rgb565,
            .append_pixdata  = qp_tft_panel_append_pixdata,
        },
    .num_window_bytes   = 1,
    .swap_window_coords = false,
    .opcodes =
        {
            .display_on         = SSD1331_CMD_DISPLAYON,
            .display_off        = SSD1331_CMD_DISPLAYOFF,
            .set_column_address = SSD1331_CMD_SETCOLUMN,
            .set_row_address    = SSD1331_CMD_SETROW,
            .enable_writes      = 0x00,
        },
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SPI

#ifdef QUANTUM_PAINTER_SSD1331_SPI_ENABLE

// Factory function for creating a handle to the SSD1331 device
painter_device_t qp_ssd1331_make_spi_device(uint16_t panel_width, uint16_t panel_height, pin_t chip_select_pin, pin_t dc_pin, pin_t reset_pin, uint16_t spi_divisor, int spi_mode) {
    for (uint32_t i = 0; i < SSD1331_NUM_DEVICES; ++i) {
        tft_panel_dc_reset_painter_device_t *driver = &ssd1331_drivers[i];
        if (!driver->base.driver_vtable) {
            driver->base.driver_vtable         = (const struct painter_driver_vtable_t *)&ssd1331_driver_vtable;
            driver->base.comms_vtable          = (const struct painter_comms_vtable_t *)&spi_comms_with_dc_vtable;
            driver->base.panel_width           = panel_width;
            driver->base.panel_height          = panel_height;
            driver->base.rotation              = QP_ROTATION_0;
            driver->base.offset_x              = 0x10; // dataset x start from 0x10(16D)
            driver->base.offset_y              = 0;
            driver->base.native_bits_per_pixel = 16; // RGB565

            // SPI and other pin configuration
            driver->base.comms_config                              = &driver->spi_dc_reset_config;
            driver->spi_dc_reset_config.spi_config.chip_select_pin = chip_select_pin;
            driver->spi_dc_reset_config.spi_config.divisor         = spi_divisor;
            driver->spi_dc_reset_config.spi_config.lsb_first       = false;
            driver->spi_dc_reset_config.spi_config.mode            = spi_mode;
            driver->spi_dc_reset_config.dc_pin                     = dc_pin;
            driver->spi_dc_reset_config.reset_pin                  = reset_pin;
            return (painter_device_t)driver;
        }
    }
    return NULL;
}

#endif // QUANTUM_PAINTER_SSD1331_SPI_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
