#include "graphic.h"

#include <stdbool.h>
#include <stdlib.h>

COLOR_TYPE get_pixel(COMPRESSED_TYPE* hashed_src, uint16_t idx) {
    return *(COLOR_TYPE*)emhashmap_get(hashed_src, (int)idx)->value;
}
// if we get compressed graphic in block we can be faster
bool get_block_compressed_graphic(compressed_gf_t* src, COLOR_TYPE* target, uint16_t start, uint16_t end) {
    uint16_t data_idx = (src->addr)->idx;
    uint16_t offset   = 0;
    for (uint16_t i = start; i < end; i++) {
            target[i] = (src->addr + offset)->rgb565;
            if (data_idx <= i) {
            offset++;

            uint16_t data_idx_next = (src->addr + offset)->idx;
            if (data_idx > data_idx_next) {
                // data not sort by data_idx return read failed.
                return false;
            }
            data_idx = data_idx_next;
        }
    }
    return true;
}
