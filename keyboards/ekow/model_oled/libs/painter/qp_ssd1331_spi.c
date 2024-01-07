// Copyright 2021 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "qp_comms.h"
#include "spi_master.h"
#include "qp_comms_spi.h"

void spi_init(void) {
    static bool is_initialised = false;
    if (!is_initialised) {
        is_initialised = true;
        // Try releasing special pins for a short time
        setPinInput(SPI_SCK_PIN);

        setPinInput(SPI_MOSI_PIN);
        chThdSleepMilliseconds(10);
#if defined(USE_GPIOV1)
        palSetPadMode(PAL_PORT(SPI_SCK_PIN), PAL_PAD(SPI_SCK_PIN), SPI_SCK_PAL_MODE);

        palSetPadMode(PAL_PORT(SPI_MOSI_PIN), PAL_PAD(SPI_MOSI_PIN), SPI_MOSI_PAL_MODE);

#else
        palSetPadMode(PAL_PORT(SPI_SCK_PIN), PAL_PAD(SPI_SCK_PIN), PAL_MODE_ALTERNATE(SPI_SCK_PAL_MODE) | PAL_OUTPUT_TYPE_PUSHPULL | PAL_OUTPUT_SPEED_HIGHEST);
        palSetPadMode(PAL_PORT(SPI_MOSI_PIN), PAL_PAD(SPI_MOSI_PIN), PAL_MODE_ALTERNATE(SPI_MOSI_PAL_MODE) | PAL_OUTPUT_TYPE_PUSHPULL | PAL_OUTPUT_SPEED_HIGHEST);
#endif
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base SPI support

bool qp_comms_spi_init(painter_device_t device) {
    painter_driver_t      *driver       = (painter_driver_t *)device;
    qp_comms_spi_config_t *comms_config = (qp_comms_spi_config_t *)driver->comms_config;

    // Initialize the SPI peripheral
    spi_init();

    // Set up CS as output high
    setPinOutput(comms_config->chip_select_pin);
    writePinHigh(comms_config->chip_select_pin);

    return true;
}

bool qp_comms_spi_start(painter_device_t device) {
    painter_driver_t      *driver       = (painter_driver_t *)device;
    qp_comms_spi_config_t *comms_config = (qp_comms_spi_config_t *)driver->comms_config;

    return spi_start(comms_config->chip_select_pin, comms_config->lsb_first, comms_config->mode, comms_config->divisor);
}

uint32_t qp_comms_spi_send_data(painter_device_t device, const void *data, uint32_t byte_count) {
    uint32_t       bytes_remaining = byte_count;
    const uint8_t *p               = (const uint8_t *)data;
    const uint32_t max_msg_length  = 1024;

    while (bytes_remaining > 0) {
        uint32_t bytes_this_loop = QP_MIN(bytes_remaining, max_msg_length);
        spi_transmit(p, bytes_this_loop);
        p += bytes_this_loop;
        bytes_remaining -= bytes_this_loop;
    }

    return byte_count - bytes_remaining;
}

void qp_comms_spi_stop(painter_device_t device) {
    painter_driver_t      *driver       = (painter_driver_t *)device;
    qp_comms_spi_config_t *comms_config = (qp_comms_spi_config_t *)driver->comms_config;
    spi_stop();
    writePinHigh(comms_config->chip_select_pin);
}

bool qp_comms_spi_dc_reset_init(painter_device_t device) {
    if (!qp_comms_spi_init(device)) {
        return false;
    }

    painter_driver_t               *driver       = (painter_driver_t *)device;
    qp_comms_spi_dc_reset_config_t *comms_config = (qp_comms_spi_dc_reset_config_t *)driver->comms_config;

    // Set up D/C as output low, if specified
    if (comms_config->dc_pin != NO_PIN) {
        setPinOutput(comms_config->dc_pin);
        writePinLow(comms_config->dc_pin);
    }

    // Set up RST as output, if specified, performing a reset in the process
    if (comms_config->reset_pin != NO_PIN) {
        setPinOutput(comms_config->reset_pin);
        writePinLow(comms_config->reset_pin);
        wait_ms(20);
        writePinHigh(comms_config->reset_pin);
        wait_ms(20);
    }

    return true;
}

uint32_t qp_comms_spi_dc_reset_send_data(painter_device_t device, const void *data, uint32_t byte_count) {
    painter_driver_t               *driver       = (painter_driver_t *)device;
    qp_comms_spi_dc_reset_config_t *comms_config = (qp_comms_spi_dc_reset_config_t *)driver->comms_config;
    writePinHigh(comms_config->dc_pin);
    return qp_comms_spi_send_data(device, data, byte_count);
}

void qp_comms_spi_dc_reset_send_command(painter_device_t device, uint8_t cmd) {
    painter_driver_t               *driver       = (painter_driver_t *)device;
    qp_comms_spi_dc_reset_config_t *comms_config = (qp_comms_spi_dc_reset_config_t *)driver->comms_config;
    writePinLow(comms_config->dc_pin);
    spi_write(cmd);
}

void qp_comms_spi_dc_reset_bulk_command_sequence(painter_device_t device, const uint8_t *sequence, size_t sequence_len) {
    for (size_t i = 0; i < sequence_len;) {
        uint8_t command   = sequence[i];
        uint8_t delay     = sequence[i + 1];
        uint8_t num_bytes = sequence[i + 2];
        qp_comms_spi_dc_reset_send_command(device, command);
        if (num_bytes > 0) {
            qp_comms_spi_send_data(device, &sequence[i + 3], num_bytes);
        }
        if (delay > 0) {
            wait_ms(delay);
        }
        i += (3 + num_bytes);
    }
}

const painter_comms_with_command_vtable_t spi_comms_with_dc_vtable = {
    .base =
        {
            .comms_init  = qp_comms_spi_dc_reset_init,
            .comms_start = qp_comms_spi_start,
            .comms_send  = qp_comms_spi_dc_reset_send_data,
            .comms_stop  = qp_comms_spi_stop,
        },
    .send_command          = qp_comms_spi_dc_reset_send_command,
    .bulk_command_sequence = qp_comms_spi_dc_reset_bulk_command_sequence,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base comms APIs

bool qp_comms_init(painter_device_t device) {
    painter_driver_t *driver = (painter_driver_t *)device;
    if (!driver->validate_ok) {
        qp_dprintf("qp_comms_init: fail (validation_ok == false)\n");
        return false;
    }

    return driver->comms_vtable->comms_init(device);
}

bool qp_comms_start(painter_device_t device) {
    painter_driver_t *driver = (painter_driver_t *)device;
    if (!driver->validate_ok) {
        qp_dprintf("qp_comms_start: fail (validation_ok == false)\n");
        return false;
    }

    return driver->comms_vtable->comms_start(device);
}

void qp_comms_stop(painter_device_t device) {
    painter_driver_t *driver = (painter_driver_t *)device;
    if (!driver->validate_ok) {
        qp_dprintf("qp_comms_stop: fail (validation_ok == false)\n");
        return;
    }

    driver->comms_vtable->comms_stop(device);
}

uint32_t qp_comms_send(painter_device_t device, const void *data, uint32_t byte_count) {
    painter_driver_t *driver = (painter_driver_t *)device;
    if (!driver->validate_ok) {
        qp_dprintf("qp_comms_send: fail (validation_ok == false)\n");
        return false;
    }

    return driver->comms_vtable->comms_send(device, data, byte_count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Comms APIs that use a D/C pin

void qp_comms_command(painter_device_t device, uint8_t cmd) {
    painter_driver_t                    *driver       = (painter_driver_t *)device;
    painter_comms_with_command_vtable_t *comms_vtable = (painter_comms_with_command_vtable_t *)driver->comms_vtable;
    comms_vtable->send_command(device, cmd);
}

void qp_comms_command_databyte(painter_device_t device, uint8_t cmd, uint8_t data) {
    qp_comms_command(device, cmd);
    qp_comms_spi_send_data(device, &data, sizeof(data));
}

uint32_t qp_comms_command_databuf(painter_device_t device, uint8_t cmd, const void *data, uint32_t byte_count) {
    qp_comms_command(device, cmd);
    return qp_comms_spi_send_data(device, data, byte_count);

}
void qp_comms_bulk_command_sequence(painter_device_t device, const uint8_t *sequence, size_t sequence_len) {
    painter_driver_t                    *driver       = (painter_driver_t *)device;
    painter_comms_with_command_vtable_t *comms_vtable = (painter_comms_with_command_vtable_t *)driver->comms_vtable;
    comms_vtable->bulk_command_sequence(device, sequence, sequence_len);
}
