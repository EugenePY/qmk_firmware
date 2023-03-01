#include <stdbool.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

extern "C" {
#include "ff.h"
}

#include "fatfs_mock.h"

bool are_same(const BYTE *a, const BYTE *b, UINT len) {
    for (int i = 1; i < len; ++i)
        if (a[i] != b[i]) return false;
    return true;
}
using ::testing::ElementsAre;

TEST(FAFS, fake_write) {
    DiskIOMock mocker(1526, 2);
    BYTE       buffer[2] = {0, 2};
    disk_write(0, buffer, 0, 1);
    BYTE read_buffer[2];
    disk_read(0, read_buffer, 0, 1);
    ASSERT_THAT(read_buffer, ElementsAre(0, 2));
}

TEST(FATFS, mount) {
    DiskIOMock mocker(256, 512);

    FATFS   fs;              /* Filesystem object */
    FIL     fil;             /* File object */
    FRESULT res;             /* API result code */
    UINT    bw;              /* Bytes written */
    BYTE    work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
    /* Format the default drive with default parameters */

    res = f_mkfs("", 0, work, sizeof(work));

    ASSERT_EQ(res, FR_OK);

    res = f_mount(&fs, "", 0);
    ASSERT_EQ(res, FR_OK);

    res = f_open(&fil, "hello.txt", FA_CREATE_NEW | FA_WRITE);
    ASSERT_EQ(res, FR_OK);

    res = f_write(&fil, "Hello, World!\r\n", 15, &bw);
    ASSERT_EQ(res, FR_OK);
    /* Close the file */
    res = f_close(&fil);
    ASSERT_EQ(res, FR_OK);


    /* Unregister work area */
    res = f_open(&fil, "hello.txt", FA_READ);
    ASSERT_EQ(res, FR_OK);
}
