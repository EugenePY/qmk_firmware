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

#define FLASHFileStartCluster 2

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
                                                  .Name            = "MODEL OLED ",
                                                  .Attributes      = FAT_FLAG_VOLUME_NAME,
                                                  .Reserved        = {0},
                                                  .CreationTime    = FAT_TIME(1, 1, 0),
                                                  .CreationDate    = FAT_DATE(14, 2, 2023),
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
                                                     .StartingCluster = FLASHFileStartCluster,
                                                     .FileSizeBytes   = FLASH_FILE_SIZE_BYTES,
                                                 }}

};

/** Starting cluster of the virtual FLASH.BIN file on disk, tracked so that the
 *  offset from the start of the data sector can be determined. On Windows
 *  systems files are usually replaced using the original file's disk clusters,
 *  while Linux appears to overwrite with an offset which must be compensated for.
 */
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

static int ReadWriteFLASHFileBlock(const uint16_t BlockNumber, uint8_t* BlockBuffer, const bool Read) {
    uint16_t FileStartBlock = DISK_BLOCK_DataStartBlock + (FLASHFileStartCluster - 2) * SECTOR_PER_CLUSTER;
    uint16_t FileEndBlock   = FileStartBlock + (FILE_SECTORS(FLASH_FILE_SIZE_BYTES) - 1);

    uint32_t FlashAddress = (uint32_t)(BlockNumber - FileStartBlock) * SECTOR_SIZE_BYTES;

    int res = CH_FAILED;
    /* Range check the write request - abort if requested block is not within the
     *
     * virtual firmware file sector range */
    if (!((BlockNumber >= FileStartBlock) && (BlockNumber <= FileEndBlock))) return res;
    if (Read) {
        for (uint32_t i = 0; i < SECTOR_SIZE_BYTES; i++) {
            BlockBuffer[i] = FLASH_BYTE(FlashAddress, i);
        }
        res = CH_SUCCESS;
    } else {
        // erase sector at the begining of write
        if ((FlashAddress % flashSectorSize(5)) == 0) {
            res = flashSectorErase(5);
        } else if ((FlashAddress % flashSectorSize(6)) == 0) {
            res = flashSectorErase(6);
        } else if ((FlashAddress % flashSectorSize(7)) == 0) {
            res = flashSectorErase(7);
        }

        // need to check the zero value.
        res = flashWrite(FLASH_ADDR(FlashAddress), (char*)BlockBuffer, SECTOR_SIZE_BYTES);
        // update the user_config_t as dirty (n frame need to be update)
        // user_config.raw      = eeconfig_read_user();
        // user_config.is_dirty = true;
        // eeconfig_update_user(user_config.raw);
    } /* Write out the mapped block of data to the device's FLASH */
    return res;
}
static uint8_t FATBlock[SECTOR_SIZE_BYTES];

void vfs_init(void) {
    UpdateFAT12ClusterEntry(FATBlock, 0, 0xF00 | BootBlock.MediaDescriptor);

    /* Cluster 1: Reserved */
    UpdateFAT12ClusterEntry(FATBlock, 1, 0xFFF);

    /* Cluster 2 onwards: Cluster chain of FLASH.BIN */
    UpdateFAT12ClusterChain(FATBlock, FLASHFileStartCluster, FILE_CLUSTERS(FLASH_FILE_SIZE_BYTES));
}

int vfs_read_fat12(const uint16_t block_idx, uint8_t* output_block_buffer) {
    uint8_t BlockBuffer[SECTOR_SIZE_BYTES];
    memset(BlockBuffer, 0x00, SECTOR_SIZE_BYTES);

    int res = CH_FAILED;
    switch (block_idx) {
        case DISK_BLOCK_BootBlock:
            memcpy(BlockBuffer, &BootBlock, sizeof(FATBootBlock_t));

            /* Add the magic signature to the end of the block */
            BlockBuffer[SECTOR_SIZE_BYTES - 2] = 0x55;
            BlockBuffer[SECTOR_SIZE_BYTES - 1] = 0xAA;
            res                                = CH_SUCCESS;
            break;

        case DISK_BLOCK_FATBlock1:
        case DISK_BLOCK_FATBlock2:
            memcpy(BlockBuffer, FATBlock, SECTOR_SIZE_BYTES);
            res = CH_SUCCESS;
            break;
        case DISK_BLOCK_RootFilesBlock:
            memcpy(BlockBuffer, FirmwareFileEntries, SECTOR_SIZE_BYTES);
            res = CH_SUCCESS;
            break;

        default:
            res = ReadWriteFLASHFileBlock(block_idx, BlockBuffer, true);
            break;
    }
    if (res == CH_SUCCESS) {
        memcpy(output_block_buffer, BlockBuffer, sizeof(BlockBuffer));
    }
    return res;
}

int vfs_write_fat12(const uint16_t block_idx, const uint8_t* input_block_buffer) {
    int res = CH_FAILED;
    switch (block_idx) {
        case DISK_BLOCK_BootBlock:
            /* Ignore writes to the boot*/
            res = CH_SUCCESS;
            break;

        case DISK_BLOCK_FATBlock2:
        case DISK_BLOCK_FATBlock1:
            memcpy(FATBlock, input_block_buffer, SECTOR_SIZE_BYTES);
            res = CH_SUCCESS;
            break;

        case DISK_BLOCK_RootFilesBlock:
            /* Copy over the updated directory entries */
            memcpy(FirmwareFileEntries, input_block_buffer, SECTOR_SIZE_BYTES);
            res = CH_SUCCESS;
            break;
        default:
            res = ReadWriteFLASHFileBlock(block_idx, (uint8_t*)input_block_buffer, false);
            break;
    }
    return res;
}
/*
static void img_empty_check(void) {
    uint8_t  input_block_buffer[SECTOR_SIZE_BYTES];
    ReadWriteFLASHFileBlock(block_idx, input_block_buffer, true);
}
*/
// Read the image from flash
