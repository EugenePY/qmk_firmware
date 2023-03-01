#pragma once

#include "ff.h"
#include "diskio.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

class DiskIOMock {
   public:
    DiskIOMock(DWORD sector_count, DWORD block_size);
    ~DiskIOMock();

    // Mock Methods
    MOCK_METHOD(DSTATUS, init_mock, (BYTE pdrv));
    MOCK_METHOD(DSTATUS, status_mock, (BYTE pdrv));
    MOCK_METHOD(DRESULT, read_mock, (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count));
    MOCK_METHOD(DRESULT, write_mock, (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count));
    MOCK_METHOD(DRESULT, ioctl_mock, (BYTE pdrv, BYTE cmd, void* buff));

    void write_fake(const BYTE* buff, LBA_t sector, UINT count);
    void read_fake(BYTE* buff, LBA_t sector, UINT count);

    DWORD sector_count;
    DWORD sector_size;

   private:
    // total volume
    BYTE* buffer;
};
