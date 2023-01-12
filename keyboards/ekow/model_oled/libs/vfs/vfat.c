#include <ch.h>
#include <hal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "vfat.h"
#include "hal_flash.h"

BaseFlash flash_driver;

static const FATBootBlock_t BootBlock = {
    .Bootstrap             = {0xEB, 0x3C, 0x90},
    .Description           = "mkdosfs",
    .SectorSize            = SECTOR_SIZE_BYTES,
    .SectorsPerCluster     = SECTOR_PER_CLUSTER,
    .ReservedSectors       = 1,
    .FATCopies             = 2,
    .RootDirectoryEntries  = (SECTOR_SIZE_BYTES / sizeof(FATDirectoryEntry_t)),
    .TotalSectors16        = LUN_MEDIA_BLOCKS,
    .MediaDescriptor       = 0xF8,
    .SectorsPerFAT         = 1,
    .SectorsPerTrack       = (LUN_MEDIA_BLOCKS % 64),
    .Heads                 = (LUN_MEDIA_BLOCKS / 64),
    .HiddenSectors         = 0,
    .TotalSectors32        = 0,
    .PhysicalDriveNum      = 0,
    .ExtendedBootRecordSig = 0x29,
    .VolumeSerialNumber    = 0x12345678,
    .VolumeLabel           = "OLED IMG  ",
    .FilesystemIdentifier  = "FAT12   ",
};

/** FAT 8.3 style directory entry, for the virtual FLASH contents file. */
static FATDirectoryEntry_t FirmwareFileEntries[] = {
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

                                           .Checksum  = FAT_CHECKSUM('F', 'L', 'A', 'S', 'H', ' ', ' ', ' ', 'B', 'I', 'N'),
                                           .Unicode1  = 'F',
                                           .Unicode2  = 'L',
                                           .Unicode3  = 'A',
                                           .Unicode4  = 'S',
                                           .Unicode5  = 'H',
                                           .Unicode6  = '.',
                                           .Unicode7  = 'B',
                                           .Unicode8  = 'I',
                                           .Unicode9  = 'N',
                                           .Unicode10 = 0,
                                           .Unicode11 = 0,
                                           .Unicode12 = 0,
                                           .Unicode13 = 0,
                                       }},

    /* MSDOS file entry for the virtual Firmware image. */
    [DISK_FILE_ENTRY_FLASH_MSDOS] = {.MSDOS_File = {
                                         .Filename        = "FLASH   ",
                                         .Extension       = "BIN",
                                         .Attributes      = 0,
                                         .Reserved        = {0},
                                         .CreationTime    = FAT_TIME(1, 1, 0),
                                         .CreationDate    = FAT_DATE(14, 2, 1989),
                                         .StartingCluster = 2,
                                         .FileSizeBytes   = FLASH_FILE_SIZE_BYTES,
                                     }}};

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

    /* Range check the write request - abort if requested block is not within the
     * virtual firmware file sector range */
    if (!((BlockNumber >= FileStartBlock) && (BlockNumber <= FileEndBlock))) return;

    uint32_t FlashAddress = (uint32_t)(BlockNumber - FileStartBlock) * SECTOR_SIZE_BYTES;

    if (Read) {
        /* Read out the mapped block of data from the device's FLASH */
        flashRead(&flash_driver, FlashAddress, SECTOR_SIZE_BYTES, BlockBuffer);
    } else {
        /* Write out the mapped block of data to the device's FLASH */
        flashProgram(&flash_driver, FlashAddress, SECTOR_SIZE_BYTES, BlockBuffer);
    }
}

void vfs_read_fat12(const uint16_t block_idx, uint8_t* output_block_buffer) {
    switch (block_idx) {
        case DISK_BLOCK_BootBlock:
        case DISK_BLOCK_FATBlock2:
        case DISK_BLOCK_FATBlock1:
            /* Ignore writes to the boot and FAT blocks */

            break;

        case DISK_BLOCK_RootFilesBlock:
            /* Copy over the updated directory entries */
            memcpy(FirmwareFileEntries, output_block_buffer, sizeof(FirmwareFileEntries));

            break;

        default:
            ReadWriteFLASHFileBlock(block_idx, output_block_buffer, false);
            break;
    }
}

void vfs_write_fat12(const uint16_t block_idx, uint8_t* input_block_buffer) {
    uint8_t BlockBuffer[SECTOR_SIZE_BYTES];
    memset(BlockBuffer, 0x00, sizeof(BlockBuffer));
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
    input_block_buffer = BlockBuffer;
}

// Read the image from flash
void open_img(uint16_t idx, uint8_t* output_buffer) {
    FATDirectoryEntry_t* img_file_info = &FirmwareFileEntries[DISK_FILE_ENTRY_FLASH_MSDOS];
    uint32_t             img_size      = img_file_info->MSDOS_File.FileSizeBytes;

    uint16_t FileStartBlock = DISK_BLOCK_DataStartBlock + (*FLASHFileStartCluster - 2) * SECTOR_PER_CLUSTER;
    uint16_t block_idx      = FileStartBlock + idx % (FILE_SECTORS(img_size) - 1);

    ReadWriteFLASHFileBlock(block_idx, output_buffer, true);
}
