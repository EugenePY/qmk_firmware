#pragma once

/* ChibiOS SPI definitions */
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN A5
#define SPI_MOSI_PIN A7
#define SPI_MISO_PIN NO_PIN // dont use MISO pin.
#define SPI_SS_PIN A4

/* OLED SPI Pins */
#define OLED_SSD_1331_DC_PIN A3
#define OLED_REST_PIN B12
#define OLED_SHWN_PIN B1
#define OLED_SS_PIN SPI_SS_PIN
#define OLED_SPI_CLK_DIVISOR 2 /* Divisor for OLED */

// OLED Hardware Defines
// OLED-Switch, oled driver configuration
#define OLED_DISPLAY_96X64
#define OLED_DISPLAY_WIDTH 96
#define OLED_DISPLAY_HEIGHT 64
#define OLED_BUFFER_TYPE uint16_t
#define OLED_MATRIX_SIZE (OLED_DISPLAY_HEIGHT / (sizeof(OLED_BUFFER_TYPE) * 8) * OLED_DISPLAY_WIDTH) // (compile time mathed)
#define OLED_BLOCK_TYPE uint8_t
#define OLED_BLOCK_COUNT (sizeof(OLED_BLOCK_TYPE) * 8)        // 32 (compile time mathed)
#define OLED_BLOCK_SIZE (OLED_MATRIX_SIZE / OLED_BLOCK_COUNT) // 32 (compile time mathed)
