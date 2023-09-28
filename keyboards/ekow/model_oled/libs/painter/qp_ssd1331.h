// Copyright 2021 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "gpio.h"
#include "qp_internal.h"


#ifndef SSD1331_NUM_DEVICES
#    define SSD1331_NUM_DEVICES 1
#endif

#ifdef QUANTUM_PAINTER_SSD1331_SPI_ENABLE
painter_device_t qp_ssd1331_make_spi_device(uint16_t panel_width, uint16_t panel_height, pin_t chip_select_pin, pin_t dc_pin, pin_t reset_pin, uint16_t spi_divisor, int spi_mode);
#endif // QUANTUM_PAINTER_SSD1351_SPI_ENABLE
