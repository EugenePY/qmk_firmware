#include QMK_KEYBOARD_H
#define IMG_COL 64
#define IMG_ROW 48
#define IMG_SIZE IMG_ROW* IMG_COL
#define IMG_BUFFER_SIZE IMG_SIZE * 2 // RGB color in 565 format

extern const uint8_t img[IMG_BUFFER_SIZE];
