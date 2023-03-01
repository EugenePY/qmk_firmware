#include <iostream>
#include <vector>
#include "ssd1331_mock.h"

// private
SPIMockDriver *spi_driver                    = nullptr;
bool           SPIMockDriver::is_initialized = false;

SPIMockDriver::SPIMockDriver() {
    spi_driver = this;
}

SPIMockDriver::~SPIMockDriver() {
    spi_driver = nullptr;
}

void setPinOutputMock(pin_t pin) {}

void spi_init(void) {
    spi_driver->spi_init_mock();
}

spi_status_t spi_write(uint8_t data) {
    return spi_driver->spi_write_mock(data);
}

spi_status_t spi_transmit(const uint8_t *data, uint16_t length) {
    return spi_driver->spi_transmit_mock(data, length);
}

spi_status_t spi_receive(uint8_t *data, uint16_t length) {
    return SPI_STATUS_SUCCESS;
}

void spi_stop(void) {
    spi_driver->spi_stop_mock();
}

// just mocking
spi_status_t spi_read(void) {
    return SPI_STATUS_SUCCESS;
}

bool spi_start(pin_t slavePin, bool lsbFirst, uint8_t mode, uint16_t divisor) {
    return spi_driver->spi_start_mock(slavePin, lsbFirst, mode, divisor);
}


