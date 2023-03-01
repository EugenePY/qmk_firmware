#include "gtest/gtest.h"

extern "C" {
#include "ssd1331.h"
#include "ssd1331_mock.h"

extern spi_status_t  _data_write(const OLED_BUFFER_TYPE* data, uint16_t length);
extern spi_status_t  _render(const OLED_BUFFER_TYPE* data, uint16_t length);
extern oled_driver_t oled_driver;
extern bool          _oled_data_write_pixel(uint16_t* rgb565_buffer, OLED_BUFFER_TYPE* oled_buffer_ptr, uint16_t length);
}

using ::testing::_;
using ::testing::InSequence;

// Testing the Mocking Driver
TEST(SSD1331, driver_init) {
    SPIMockDriver spi_driver{};
    ASSERT_SPI_DRIVER_INITIAL(spi_driver);
    spi_init();
}
