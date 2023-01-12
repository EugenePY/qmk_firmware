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

TEST(SSD1331, driver_transmit) {
    SPIMockDriver spi_driver{};
    const uint8_t data[2] = {0x1, 0x2};
    EXPECT_CALL(spi_driver, spi_transmit_mock(data, 2)).WillOnce(testing::Return(SPI_STATUS_SUCCESS));
    spi_status_t result = spi_transmit(data, 2);
    ASSERT_TRUE(result == SPI_STATUS_SUCCESS);
}

TEST(SSD1331, oled_init) {
    SPIMockDriver spi_driver{};
    InSequence    s;
    ASSERT_SPI_DRIVER_INITIAL(spi_driver);

    EXPECT_CALL(spi_driver, spi_start_mock(_, _, _, _)).WillOnce(testing::Return(true));
    EXPECT_CALL(spi_driver, spi_transmit_mock(_, _)).WillOnce(testing::Return(SPI_STATUS_SUCCESS));
    EXPECT_CALL(spi_driver, spi_stop_mock());

    // two session call.
    EXPECT_CALL(spi_driver, spi_start_mock(_, _, _, _)).WillOnce(testing::Return(true));
    EXPECT_CALL(spi_driver, spi_transmit_mock(_, _)).WillOnce(testing::Return(SPI_STATUS_SUCCESS));
    EXPECT_CALL(spi_driver, spi_stop_mock());

    bool result = oled_init(OLED_ROTATION_0);
    EXPECT_TRUE(result);
}

TEST(SSD1331, _data_write) {
    SPIMockDriver spi_driver{};

    InSequence s;
    EXPECT_CALL(spi_driver, spi_start_mock(_, _, _, _)).WillOnce(testing::Return(true));

    EXPECT_CALL(spi_driver, spi_write_mock(0xff)).WillOnce(testing::Return(SPI_STATUS_SUCCESS));
    EXPECT_CALL(spi_driver, spi_write_mock(0x11)).WillOnce(testing::Return(SPI_STATUS_SUCCESS));
    EXPECT_CALL(spi_driver, spi_write_mock(0x44)).WillOnce(testing::Return(SPI_STATUS_SUCCESS));
    EXPECT_CALL(spi_driver, spi_write_mock(0x22)).WillOnce(testing::Return(SPI_STATUS_SUCCESS));
    EXPECT_CALL(spi_driver, spi_stop_mock());

    const uint16_t data[] = {0x11ff, 0x2244};
    spi_status_t   a      = _data_write(data, 2);
    EXPECT_TRUE(a == SPI_STATUS_SUCCESS);
}
/*
TEST(SSD1331, _oled_data_write_pixel) {
    SPIMockDriver spi_driver{};
    InSequence    s;
    EXPECT_CALL(spi_driver, spi_start_mock(_, _, _, _)).WillOnce(testing::Return(true));
    EXPECT_CALL(spi_driver, spi_write_mock(0x11)).WillRepeatedly(testing::Return(SPI_STATUS_SUCCESS));

    EXPECT_CALL(spi_driver, spi_stop_mock());

    OLED_BUFFER_TYPE data[]  = {0xffff}; // all 16 pixel has data.
    uint16_t         color[] = {0x1111, 0x1111, 0x1111, 0x1111, 
                                0x1111, 0x1111, 0x1111, 0x1111, 
                                0x1111, 0x1111, 0x1111, 0x1111, 
                                0x1111, 0x1111, 0x1111, 0x1111, 
                                0x1111, 0x1111, 0x1111, 0x1111};
    bool a = _oled_data_write_pixel(&color[0], &data[0], 1);
    EXPECT_TRUE(a);
}
*/

TEST(SSD1331, _oled_render) {
    SPIMockDriver spi_driver{};
    InSequence    s;
    EXPECT_CALL(spi_driver, spi_start_mock(_, _, _, _)).WillOnce(testing::Return(true));
    EXPECT_CALL(spi_driver, spi_write_mock(0x22)).WillOnce(testing::Return(SPI_STATUS_SUCCESS));
    EXPECT_CALL(spi_driver, spi_write_mock(0x11)).WillOnce(testing::Return(SPI_STATUS_SUCCESS));
    EXPECT_CALL(spi_driver, spi_write_mock(0xcd)).WillOnce(testing::Return(SPI_STATUS_SUCCESS));
    EXPECT_CALL(spi_driver, spi_write_mock(0xab)).WillOnce(testing::Return(SPI_STATUS_SUCCESS));
    EXPECT_CALL(spi_driver, spi_stop_mock());

    const uint16_t color[] = {0x1122, 0xabcd};
    ssd1331_oled_render(color, 2);
    EXPECT_TRUE(color[0] == 0x1122);
}
