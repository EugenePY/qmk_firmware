#include QMK_KEYBOARD_H

#define IMG_SIZE 96 * 64
#define IMG_BUFFER_SIZE IMG_SIZE * 2 // RGB color in 565 format
extern const uint8_t PROGMEM img[IMG_BUFFER_SIZE];
