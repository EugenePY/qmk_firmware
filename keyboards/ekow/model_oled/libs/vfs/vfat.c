#include <ch.h>
#include <hal.h>
#include <stdint.h>
#include "quantum.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "vfat.h"
#include "flash.h"
#include "eeconfig.h"

#include "model_oled.h"
#include "img/icon.h"

#define FLASH_ADDR(offset) (FLASH_BASE_ADDR + (offset))
#define FLASH_PTR(offset) ((__IO uint8_t*)FLASH_ADDR(offset))

#define FLASH_BYTE(loc, offset) (*(FLASH_PTR(((uint32_t)loc) + ((uint32_t)offset))))

extern user_config_t user_config;

static const FATBootBlock_t BootBlock = {
    .Bootstrap             = {0xEB, 0x3C, 0x90},
    .Description           = "mkdosfs",
    .SectorSize            = SECTOR_SIZE_BYTES,
    .SectorsPerCluster     = SECTOR_PER_CLUSTER,
    .ReservedSectors       = 1,
    .FATCopies             = 2,
    .RootDirectoryEntries  = ((SECTOR_SIZE_BYTES) / sizeof(FATDirectoryEntry_t)), // First 32 Bytes in sector
    .TotalSectors16        = LUN_MEDIA_BLOCKS,
    .MediaDescriptor       = 0xF8, // Unremovable
    .SectorsPerFAT         = 1,
    .SectorsPerTrack       = (LUN_MEDIA_BLOCKS % 64), // not greater than 64
    .Heads                 = (LUN_MEDIA_BLOCKS / 64),
    .HiddenSectors         = 0,
    .TotalSectors32        = 0,
    .PhysicalDriveNum      = 0,
    .ExtendedBootRecordSig = 0x29,
    .VolumeSerialNumber    = 0x00000001,
    .VolumeLabel           = "OLED IMG  ",
    .FilesystemIdentifier  = "FAT12   ",
};

/** FAT 8.3 style directory entry, for the virtual FLASH contents file. */
static FATDirectoryEntry_t
    FirmwareFileEntries[SECTOR_SIZE_BYTES / sizeof(FATDirectoryEntry_t)] =
        {
            /* Root volume label entry; disk label is contained in the Filename and
             * Extension fields (concatenated) with a special attribute flag - other
             * fields are ignored. Should be the same as the label in the boot block.
             */
            [DISK_FILE_ENTRY_VolumeID] = {.MSDOS_Directory =
                                              {
                                                  .Name            = "OLED IMG  ",
                                                  .Attributes      = FAT_FLAG_VOLUME_NAME,
                                                  .Reserved        = {0},
                                                  .CreationTime    = 0,
                                                  .CreationDate    = 0,
                                                  .StartingCluster = 0,
                                                  .Reserved2       = 0,
                                              }},

            /* VFAT Long File Name entry for the virtual firmware file; required to
             * prevent corruption from systems that are unable to detect the device
             * as being a legacy MSDOS style FAT12 volume. */
            [DISK_FILE_ENTRY_FLASH_LFN] = {.VFAT_LongFileName =
                                               {
                                                   .Ordinal   = 1 | FAT_ORDINAL_LAST_ENTRY,
                                                   .Attribute = FAT_FLAG_LONG_FILE_NAME,
                                                   .Reserved1 = 0,
                                                   .Reserved2 = 0,

                                                   .Checksum  = FAT_CHECKSUM('O', 'L', 'E', 'D', ' ', ' ', ' ', ' ', 'I', 'M', 'G'),
                                                   .Unicode1  = 'O',
                                                   .Unicode2  = 'L',
                                                   .Unicode3  = 'E',
                                                   .Unicode4  = 'D',
                                                   .Unicode5  = '.',
                                                   .Unicode6  = 'I',
                                                   .Unicode7  = 'M',
                                                   .Unicode8  = 'G',
                                                   .Unicode9  = 0,
                                                   .Unicode10 = 0,
                                                   .Unicode11 = 0,
                                                   .Unicode12 = 0,
                                                   .Unicode13 = 0,
                                               }},

            /* MSDOS file entry for the virtual Firmware image. */
            [DISK_FILE_ENTRY_FLASH_MSDOS] = {.MSDOS_File =
                                                 {
                                                     .Filename        = "OLED    ",
                                                     .Extension       = "IMG",
                                                     .Attributes      = 0,
                                                     .Reserved        = {0},
                                                     .CreationTime    = FAT_TIME(1, 1, 0),
                                                     .CreationDate    = FAT_DATE(14, 2, 2023),
                                                     .StartingCluster = 2,
                                                     .FileSizeBytes   = FLASH_FILE_SIZE_BYTES,
                                                 }}

};

/** Starting cluster of the virtual FLASH.BIN file on disk, tracked so that the
 *  offset from the start of the data sector can be determined. On Windows
 *  systems files are usually replaced using the original file's disk clusters,
 *  while Linux appears to overwrite with an offset which must be compensated for.
 */
static const uint16_t* FLASHFileStartCluster = &FirmwareFileEntries[DISK_FILE_ENTRY_FLASH_MSDOS].MSDOS_File.StartingCluster;

/** Updates a FAT12 cluster entry in the FAT file table with the specified next
 *  chain index. If the cluster is the last in the file chain, the magic value
 *  \c 0xFFF should be used.
 *
 *  \note FAT data cluster indexes are offset by 2, so that cluster 2 is the
 *        first file data cluster on the disk. See the FAT specification.
 *
 *  \param[out]  FATTable    Pointer to the FAT12 allocation table
 *  \param[in]   Index       Index of the cluster entry to update
 *  \param[in]   ChainEntry  Next cluster index in the file chain
 */
static void UpdateFAT12ClusterEntry(uint8_t* const FATTable, const uint16_t Index, const uint16_t ChainEntry) {
    /* Calculate the starting offset of the cluster entry in the FAT12 table */
    uint8_t FATOffset   = (Index + (Index >> 1));
    bool    UpperNibble = ((Index & 1) != 0);

    /* Check if the start of the entry is at an upper nibble of the byte, fill
     * out FAT12 entry as required */
    if (UpperNibble) {
        FATTable[FATOffset]     = (FATTable[FATOffset] & 0x0F) | ((ChainEntry & 0x0F) << 4);
        FATTable[FATOffset + 1] = (ChainEntry >> 4);
    } else {
        FATTable[FATOffset]     = ChainEntry;
        FATTable[FATOffset + 1] = (FATTable[FATOffset + 1] & 0xF0) | (ChainEntry >> 8);
    }
}

/** Updates a FAT12 cluster chain in the FAT file table with a linear chain of
 *  the specified length.
 *
 *  \note FAT data cluster indexes are offset by 2, so that cluster 2 is the
 *        first file data cluster on the disk. See the FAT specification.
 *
 *  \param[out]  FATTable     Pointer to the FAT12 allocation table
 *  \param[in]   Index        Index of the start of the cluster chain to update
 *  \param[in]   ChainLength  Length of the chain to write, in clusters
 */
static void UpdateFAT12ClusterChain(uint8_t* const FATTable, const uint16_t Index, const uint8_t ChainLength) {
    for (uint8_t i = 0; i < ChainLength; i++) {
        uint16_t CurrentCluster = Index + i;
        uint16_t NextCluster    = CurrentCluster + 1;

        /* Mark last cluster as end of file */
        if (i == (ChainLength - 1)) NextCluster = 0xFFF;

        UpdateFAT12ClusterEntry(FATTable, CurrentCluster, NextCluster);
    }
}

static void ReadWriteFLASHFileBlock(const uint16_t BlockNumber, uint8_t* BlockBuffer, const bool Read) {
    uint16_t FileStartBlock = DISK_BLOCK_DataStartBlock + (*FLASHFileStartCluster - 2) * SECTOR_PER_CLUSTER;
    uint16_t FileEndBlock   = FileStartBlock + (FILE_SECTORS(FLASH_FILE_SIZE_BYTES) - 1);

    uint32_t FlashAddress = (uint32_t)(BlockNumber - FileStartBlock) * SECTOR_SIZE_BYTES;
    /* Range check the write request - abort if requested block is not within the
     * virtual firmware file sector range */
    if (!((BlockNumber >= FileStartBlock) && (BlockNumber <= FileEndBlock))) return;

    if (Read) {
        for (uint32_t i = 0; i < SECTOR_SIZE_BYTES; i++) {
            BlockBuffer[i] = FLASH_BYTE(FlashAddress, i);
        }
    } else {
        // erase sector at the begining of write
        if ((FlashAddress % flashSectorSize(5)) == 0) {
            flashSectorErase(5);
        } else if ((FlashAddress % flashSectorSize(6)) == 0) {
            flashSectorErase(6);
        } else if ((FlashAddress % flashSectorSize(7)) == 0) {
            flashSectorErase(7);
        }

        // need to check the zero value.
        flashWrite(FLASH_ADDR(FlashAddress), (char*)BlockBuffer, SECTOR_SIZE_BYTES);
        // update the user_config_t as dirty (n frame need to be update)
        user_config.raw      = eeconfig_read_user();
        user_config.is_dirty = true;
        eeconfig_update_user(user_config.raw);

    } /* Write out the mapped block of data to the device's FLASH */
}

void vfs_read_fat12(const uint16_t block_idx, uint8_t* output_block_buffer) {
    uint8_t BlockBuffer[SECTOR_SIZE_BYTES];
    memset(BlockBuffer, 0x00, SECTOR_SIZE_BYTES);
    switch (block_idx) {
        case DISK_BLOCK_BootBlock:
            memcpy(BlockBuffer, &BootBlock, sizeof(FATBootBlock_t));

            /* Add the magic signature to the end of the block */
            BlockBuffer[SECTOR_SIZE_BYTES - 2] = 0x55;
            BlockBuffer[SECTOR_SIZE_BYTES - 1] = 0xAA;

            break;

        case DISK_BLOCK_FATBlock1:
        case DISK_BLOCK_FATBlock2:
            /* Cluster 0: Media type/Reserved */
            UpdateFAT12ClusterEntry(BlockBuffer, 0, 0xF00 | BootBlock.MediaDescriptor);

            /* Cluster 1: Reserved */
            UpdateFAT12ClusterEntry(BlockBuffer, 1, 0xFFF);

            /* Cluster 2 onwards: Cluster chain of FLASH.BIN */
            UpdateFAT12ClusterChain(BlockBuffer, *FLASHFileStartCluster, FILE_CLUSTERS(FLASH_FILE_SIZE_BYTES));
            break;

        case DISK_BLOCK_RootFilesBlock:
            memcpy(BlockBuffer, FirmwareFileEntries, sizeof(FirmwareFileEntries));
            break;

        default:
            ReadWriteFLASHFileBlock(block_idx, BlockBuffer, true);
            break;
    }
    memcpy(output_block_buffer, BlockBuffer, sizeof(BlockBuffer));
}

void vfs_write_fat12(const uint16_t block_idx, uint8_t* input_block_buffer) {
    switch (block_idx) {
        case DISK_BLOCK_BootBlock:
        case DISK_BLOCK_FATBlock2:
        case DISK_BLOCK_FATBlock1:
            /* Ignore writes to the boot and FAT blocks */
            break;

        case DISK_BLOCK_RootFilesBlock:
            /* Copy over the updated directory entries */
            memcpy(FirmwareFileEntries, input_block_buffer, sizeof(FirmwareFileEntries));
            break;
        default:
            ReadWriteFLASHFileBlock(block_idx, input_block_buffer, false);
            break;
    }
}
/*
static void img_empty_check(void) {
    uint8_t  input_block_buffer[SECTOR_SIZE_BYTES];
    ReadWriteFLASHFileBlock(block_idx, input_block_buffer, true);
}
*/
// Read the image from flash

void img_init(void) {
    uint16_t FileStartBlock = DISK_BLOCK_DataStartBlock + (*FLASHFileStartCluster - 2) * SECTOR_PER_CLUSTER;
    user_config.raw         = eeconfig_read_user();
    if (user_config.img_is_empty) {
        // write the default img to the address
        size_t   size      = IMG_BUFFER_SIZE;
        uint16_t block_idx = FileStartBlock;
        uint8_t  input_block_buffer[SECTOR_SIZE_BYTES];

        while (size > 0) {
            size_t n = MIN(size, SECTOR_SIZE_BYTES);
            memset(input_block_buffer, 0, SECTOR_SIZE_BYTES);
            memcpy(input_block_buffer, &img[IMG_BUFFER_SIZE - size], n);
            ReadWriteFLASHFileBlock(block_idx, input_block_buffer, false);
            size -= n;
            ++block_idx;
        }
        img_update(true, 0, 1);
    }
}

static size_t get_img_frame(uint8_t* start_addr) {
    size_t   n_frame   = 0;
    size_t   size      = 0;
    bool stop = false;
    while (!stop) {
        if ((*start_addr != 0xff) | (size > 6144 * 30)) {
            stop = true;
        } else {
            ++size;
        }
        if ((size % IMG_BUFFER_SIZE == 0) | stop) {
            ++n_frame;
        }
    }
    return n_frame;
}

void img_update(bool is_updated, bool is_dirty, size_t n_frame) {
    // should be call when the user updated the image
    user_config.raw          = eeconfig_read_user();
    user_config.img_is_empty = !is_updated;
    user_config.n_frame      = n_frame;
    user_config.is_dirty     = is_dirty;
    eeconfig_update_user(user_config.raw);
}

void get_current_img(img_t* img_ptr) {
    user_config.raw = eeconfig_read_user();
    if (user_config.img_is_empty) {
        // write the default img to the address
        img_ptr->start_addr = (uint32_t)&img;
        img_ptr->n_frame    = 1;
    } else {
        img_ptr->start_addr = FLASH_BASE_ADDR;
        if (user_config.is_dirty) {
            user_config.n_frame  = get_img_frame((uint8_t*)FLASH_BASE_ADDR);
            user_config.is_dirty = false;
            eeconfig_update_user(user_config.raw);
        }
        img_ptr->n_frame = user_config.n_frame;
    }
}
