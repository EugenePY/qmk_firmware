#pragma once

/* ChibiOS SPI definitions */
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN A5      // this should be the driver pin
#define SPI_MOSI_PIN A7     // this should be the driver pin
#define SPI_MISO_PIN NO_PIN // MISO is useless in ssd1331 spi mode.
#define SPI_SS_PIN B1       // can be any gpio

/* OLED SPI Pins */
#define OLED_SSD_1331_DC_PIN B0 // can be any gpio
#define OLED_REST_PIN C13
#define OLED_SHWN_PIN C14
#define OLED_SS_PIN SPI_SS_PIN
#define OLED_SPI_CLK_DIVISOR 2 /* Divisor for OLED */

#define OLED_WIDTH 64
#define OLED_HEIGHT 48
#define OLED_BUFFER_SIZE OLED_WIDTH * OLED_HEIGHT * 2
#define USE_MSD_PROTOCOL // use MSC for updating image/gif
#define SSD1331_NUM_DEVICES 1
#define QUANTUM_PAINTER_SUPPORTS_NATIVE_COLORS TRUE
