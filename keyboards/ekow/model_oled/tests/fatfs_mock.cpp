#include <cstring>
#include "fatfs_mock.h"

DiskIOMock *mocker = nullptr;

DiskIOMock::DiskIOMock(DWORD sector_count, DWORD sector_size) {
    mocker       = this;
    this->buffer = (BYTE *)malloc(sector_count * sector_size);
    memset(this->buffer, 0, sector_count * sector_size); // iniital all zeros

    this->sector_size= sector_size;
    this->sector_count = sector_count;
}

void DiskIOMock::read_fake(BYTE *buff, LBA_t sector, UINT count) {
    memcpy(buff, &this->buffer[sector * this->sector_size], (count) * this->sector_size);
}

void DiskIOMock::write_fake(const BYTE *buff, LBA_t sector, UINT count) {
    memcpy(&this->buffer[sector * this->sector_size], buff, (count) * this->sector_size);
}

DiskIOMock::~DiskIOMock() {
    mocker = nullptr;
    free(this->buffer);
}

DSTATUS disk_initialize(BYTE pdrv) {
    return mocker->init_mock(pdrv);
};

DSTATUS disk_status(BYTE pdrv) {
    return mocker->status_mock(pdrv);
};

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
    mocker->read_fake(buff, sector, count);
    return mocker->read_mock(pdrv, buff, sector, count);
};

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
    mocker->write_fake(buff, sector, count);
    return mocker->write_mock(pdrv, buff, sector, count);
};

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    (void)buff;
    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *((DWORD *)buff) = mocker->sector_count;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((WORD *)buff) = mocker->sector_size;
            return RES_OK;
        default:
            return RES_PARERR;
    }
    return RES_PARERR;
};

DWORD get_fattime(void) {
    return ((uint32_t)0 | (1 << 16)) | (1 << 21);
}
