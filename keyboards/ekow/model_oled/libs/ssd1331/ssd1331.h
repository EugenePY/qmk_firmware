#pragma once
#include "oled_driver.h"

// Misc defines
#ifndef OLED_BLOCK_COUNT
#    define OLED_BLOCK_COUNT (sizeof(OLED_BLOCK_TYPE) * 8)
#endif

#ifndef OLED_BLOCK_SIZE
#    define OLED_BLOCK_SIZE (OLED_MATRIX_SIZE / OLED_BLOCK_COUNT)
#endif

#ifndef OLED_BUFFER_TYPE
#    define OLED_BUFFER_TYPE uint16_t
#endif

#define OLED_ALL_BLOCKS_MASK (((((OLED_BLOCK_TYPE)1 << (OLED_BLOCK_COUNT - 1)) - 1) << 1) | 1)

// Testing
#ifndef OLED_TEST
#    define STATIC static
#else
#    define STATIC
#endif

typedef struct {
    OLED_BUFFER_TYPE* oled_cursor;
    bool              oled_scrolling;
    bool              oled_initialized;
    bool              oled_active;
    OLED_BLOCK_TYPE   oled_dirty;
    oled_rotation_t   oled_rotation_width;
} oled_driver_t;

// D/C pull high as Data, low as Command
// Timing Delays
#define SSD1331_DELAYS_HWFILL (3) //!< Fill delay
#define SSD1331_DELAYS_HWLINE (1) //!< Line delay

// SSD1331 Commands
#define SSD1331_CMD_DRAWLINE 0x21              //!< Draw line
#define SSD1331_CMD_DRAWRECT 0x22              //!< Draw rectangle
#define SSD1331_CMD_FILL 0x26                  //!< Fill enable/disable
#define SSD1331_CMD_SETCOLUMN 0x15             //!< Set column address
#define SSD1331_CMD_SETROW 0x75                //!< Set row adress
#define SSD1331_CMD_CONTRASTA 0x81             //!< Set contrast for color A
#define SSD1331_CMD_CONTRASTB 0x82             //!< Set contrast for color B
#define SSD1331_CMD_CONTRASTC 0x83             //!< Set contrast for color C
#define SSD1331_CMD_MASTERCURRENT 0x87         //!< Master current control
#define SSD1331_CMD_SETREMAP 0xA0              //!< Set re-map & data format
#define SSD1331_CMD_STARTLINE 0xA1             //!< Set display start line
#define SSD1331_CMD_DISPLAYOFFSET 0xA2         //!< Set display offset
#define SSD1331_CMD_NORMALDISPLAY 0xA4         //!< Set display to normal mode
#define SSD1331_CMD_DISPLAYALLON 0xA5          //!< Set entire display ON
#define SSD1331_CMD_DISPLAYALLOFF 0xA6         //!< Set entire display OFF
#define SSD1331_CMD_INVERTDISPLAY 0xA7         //!< Invert display
#define SSD1331_CMD_SETMULTIPLEX 0xA8          //!< Set multiplex ratio
#define SSD1331_CMD_DIMMODESETTING 0xAB        //! Dim Mode Setting
#define SSD1331_CMD_DIMDISPLAY 0xAC            //! Dim Display
#define SSD1331_CMD_SETMASTER 0xAD             //!< Set master configuration
#define SSD1331_CMD_DISPLAYOFF 0xAE            //!< Display OFF (sleep mode)
#define SSD1331_CMD_DISPLAYON 0xAF             //!< Normal Brightness Display ON
#define SSD1331_CMD_POWERMODE 0xB0             //!< Power save mode
#define SSD1331_CMD_PRECHARGE 0xB1             //!< Phase 1 and 2 period adjustment
#define SSD1331_CMD_CLOCKDIV 0xB3              //!< Set display clock divide ratio/oscillator frequency
#define SSD1331_CMD_ENABLELINEARGRAYSCALE 0xB9 //! enable linear gray scale table.
#define SSD1331_CMD_PRECHARGEA 0x8A            //!< Set second pre-charge speed for color A
#define SSD1331_CMD_PRECHARGEB 0x8B            //!< Set second pre-charge speed for color B
#define SSD1331_CMD_PRECHARGEC 0x8C            //!< Set second pre-charge speed for color C
#define SSD1331_CMD_PRECHARGELEVEL 0xBB        //!< Set pre-charge voltage
#define SSD1331_CMD_VCOMH 0xBE                 //!< Set Vcomh voltage

bool is_oled_driver_actived(void);
bool is_oled_driver_init(void);
bool ssd1331_oled_setup_window(void);

void ssd1331_oled_render(const uint8_t* img, uint16_t length);
void oled_write_rgb_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void ssd1331_oled_write_raw_byte(const OLED_BUFFER_TYPE* data, uint16_t index);
bool ssd1331_oled_write_window(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
