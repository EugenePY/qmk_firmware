#pragma once

#define OLED_SSD_1331_DC_PIN 0
#define OLED_REST_PIN 1
#define OLED_SHWN_PIN 2
#define OLED_SS_PIN 3
#define OLED_SPI_CLK_DIVISOR 2

// OLED Hardware Defines
#define OLED_DISPLAY_WIDTH 96
#define OLED_DISPLAY_HEIGHT 64
#define OLED_MATRIX_SIZE (OLED_DISPLAY_HEIGHT / 8 * OLED_DISPLAY_WIDTH) // (compile time mathed)
#define OLED_BLOCK_TYPE uint16_t
#define OLED_BLOCK_COUNT (sizeof(OLED_BLOCK_TYPE) * 8)        // 32 (compile time mathed)
#define OLED_BLOCK_SIZE (OLED_MATRIX_SIZE / OLED_BLOCK_COUNT) // 32 (compile time mathed)



