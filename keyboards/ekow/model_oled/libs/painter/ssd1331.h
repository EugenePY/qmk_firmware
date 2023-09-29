#pragma once
#include "qp.h"

typedef struct {
    bool              oled_initialized;
    bool              oled_active;
    bool              oled_dimed;
    painter_device_t* device_pt;
} oled_driver_t;

bool oled_init(painter_device_t* device_pt);
bool oled_on(void);
bool oled_off(void);
bool oled_dim(void);
void oled_clear(void);
bool oled_toggle(void);
bool oled_shutdown(void);
