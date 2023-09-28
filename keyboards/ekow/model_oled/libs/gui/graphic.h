#pragma once

#include "hashmap.h"
#include <stdint.h>

#define WIDTH_TYPE uint8_t
#define HEIGHT_TYPE uint8_t
#define PIXEL_TYPE rgb565_t
#define IDX(x, y, width) x + y* width

typedef union rgb565 {
    uint16_t raw;
    struct rgb {
        uint8_t r : 5;
        uint8_t g : 6;
        uint8_t b : 5;
    } __attribute__((packed)) rgb_t;
} rgb565_t;

uint8_t getR(PIXEL_TYPE pixel);
uint8_t getG(PIXEL_TYPE piexl);
uint8_t getB(PIXEL_TYPE pixel);

#define COLOR_TYPE PIXEL_TYPE
#define BLACK (PIXEL_TYPE)((uint16_t)0xFFAAU)

// mapping 2d indxing to single index
#define IDX_FLAT(i, j, width) i + j* width
#define IDX_2D(idx, width) \
    { .x = idx % width, .y = idx / width }

#define COMPRESSED_TYPE HashMap

typedef struct {
    WIDTH_TYPE  x;
    HEIGHT_TYPE y;
} idx_2d_t;

// compression graphic format
typedef struct {
    uint16_t   idx; // i  + j * row(i=idx of col, j idx of row)
    COLOR_TYPE rgb565;
} compressed_data_t;

typedef struct {
    WIDTH_TYPE         width;
    HEIGHT_TYPE        height;
    compressed_data_t* addr; //
} compressed_gf_t;

bool       read_compressed_graphic(compressed_gf_t* src, COLOR_TYPE* target, const uint16_t start, uint16_t end);
COLOR_TYPE get_pixel(COMPRESSED_TYPE* hashed_src, uint16_t idx);

typedef struct {
    WIDTH_TYPE  width;
    HEIGHT_TYPE height;
    uint16_t    n_frame;
} animate_t;
