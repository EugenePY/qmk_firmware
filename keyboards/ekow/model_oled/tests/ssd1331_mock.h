#pragma once

#include "stdbool.h"
#include "stdint.h"

// pal mock
extern bool     pin_config_output[];
typedef uint8_t pin_t;

#define writePinHigh(pin)
#define writePinLow(pin)
#define setPinOutput(pin) setPinOutputMock(pin)

// SPI mock
typedef uint8_t spi_status_t;
#define SPI_STATUS_SUCCESS (0)
#define SPI_STATUS_ERROR (-1)
#define SPI_STATUS_TIMEOUT (-2)
#define SPI_TIMEOUT_IMMEDIATE (0)
#define SPI_TIMEOUT_INFINITE (0xFFFF)

#ifdef __cplusplus

extern "C++" {
#    include "gtest/gtest.h"
#    include "gmock/gmock.h"
}

class SPIMockDriver {
   public:
    SPIMockDriver();
    ~SPIMockDriver();
    MOCK_METHOD(void, spi_init_mock, ());
    MOCK_METHOD(spi_status_t, spi_write_mock, (uint8_t data));
    MOCK_METHOD(bool, spi_start_mock, (pin_t slavePin, bool lsbFirst, uint8_t mode, uint16_t divisor));
    MOCK_METHOD(bool, spi_transmit_mock, (const uint8_t* data, uint16_t length));
    MOCK_METHOD(void, spi_stop_mock, ());

   private:
    static std::vector<uint8_t> data_buffer;
    static bool                 is_initialized;
    static bool                 pins[];
};

#    define ASSERT_SPI_DRIVER_INITIAL(test_driver) EXPECT_CALL(test_driver, spi_init_mock())

extern "C" {
#endif

void setPinOutputMock(pin_t pin);

void spi_init(void);

bool spi_start(pin_t slavePin, bool lsbFirst, uint8_t mode, uint16_t divisor);

spi_status_t spi_write(uint8_t data);

spi_status_t spi_read(void);

spi_status_t spi_transmit(const uint8_t* data, uint16_t length);

spi_status_t spi_receive(uint8_t* data, uint16_t length);

void spi_stop(void);

// wait us
void wait_us(uint16_t duration);

#ifdef __cplusplus
};
#endif

#ifdef __cplusplus

#endif
