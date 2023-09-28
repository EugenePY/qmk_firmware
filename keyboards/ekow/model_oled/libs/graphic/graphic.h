#pragma once
#include <stdint.h>
#define WIDHT_TYPE uint8_t
#define HEIGHT_TYPE uint8_t
#define COLOR_TYPE uint16_t
#define BLACK 0x0000
// compression graphic format
typedef struct {
    WIDHT_TYPE  x;
    HEIGHT_TYPE y;
} idx_t;

typedef struct {
    idx_t    idx; // i  + j * row(i=idx of col, j idx of row)
    COLOR_TYPE rgb565;
} compressed_data_t;

typedef struct {
    WIDHT_TYPE         width;
    HEIGHT_TYPE        height;
    compressed_data_t* addr; // this must be sort by idx.
} compressed_gf_t;

bool read_compressed_graphic(compressed_gf_t* src, COLOR_TYPE* target, const idx_t start_idx, const idx_t end_idx);
