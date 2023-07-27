#include <stdbool.h>
#include "graphic.h"

static uint16_t idx_to_value(HEIGHT_TYPE h, idx_t idx) {
    return idx.x + idx.y * h;
}

static bool _read_compressed_pixel(compressed_gf_t* src, idx_t idx) {
    return true;
}

bool read_compressed_graphic(compressed_gf_t* src, COLOR_TYPE* target, const idx_t start_idx, const idx_t end_idx) {
    uint16_t start = idx_to_value(src->width, start_idx);
    uint16_t end   = idx_to_value(src->width, end_idx);
    if (start > end) {
        return false;
    }
    uint16_t data_idx = idx_to_value(src->width, (src->addr)->idx);
    uint16_t offset   = 0;
    for (uint16_t i = start; i < end; i++) {
        if (data_idx > i) {
            target[i] = BLACK;
        } else {
            target[i] = (src->addr + offset)->rgb565;
            offset++;

            uint16_t data_idx_next = idx_to_value(src->width, (src->addr + offset)->idx);
            if (data_idx > data_idx_next) {
                // data not sort by data_idx return read failed.
                return false;
            }
            data_idx = data_idx_next;
        }
    }
    return true;
}
