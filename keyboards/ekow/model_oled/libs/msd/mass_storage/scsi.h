// SCSI spcs
#pragma once

/* Command statuses */
#define MSD_COMMAND_PASSED 0x00
#define MSD_COMMAND_FAILED 0x01
#define MSD_COMMAND_PHASE_ERROR 0x02

/* SCSI Comand group code */
#define SCSI_CMD_6 0x0
#define SCSI_CMD_10 (0x1 << 2 | 0x1 << 3)

/* SCSI commands */
#define SCSI_CMD_REQUEST_SENSE 0x03
#define SCSI_CMD_TEST_UNIT_READY 0x00
#define SCSI_CMD_FORMAT_UNIT 0x04
#define SCSI_CMD_INQUIRY 0x12
#define SCSI_CMD_MODE_SENSE_6 0x1A
#define SCSI_CMD_START_STOP_UNIT 0x1B
#define SCSI_CMD_SEND_DIAGNOSTIC 0x1D
#define SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL 0x1E
#define SCSI_CMD_READ_FORMAT_CAPACITIES 0x23
#define SCSI_CMD_READ_CAPACITY_10 0x25
#define SCSI_CMD_READ_10 0x28
#define SCSI_CMD_WRITE_10 0x2A
#define SCSI_CMD_VERIFY_10 0x2F

/* SCSI sense keys */
#define SCSI_SENSE_KEY_GOOD 0x00
#define SCSI_SENSE_KEY_RECOVERED_ERROR 0x01
#define SCSI_SENSE_KEY_NOT_READY 0x02
#define SCSI_SENSE_KEY_MEDIUM_ERROR 0x03
#define SCSI_SENSE_KEY_HARDWARE_ERROR 0x04
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST 0x05
#define SCSI_SENSE_KEY_UNIT_ATTENTION 0x06
#define SCSI_SENSE_KEY_DATA_PROTECT 0x07
#define SCSI_SENSE_KEY_BLANK_CHECK 0x08
#define SCSI_SENSE_KEY_VENDOR_SPECIFIC 0x09
#define SCSI_SENSE_KEY_COPY_ABORTED 0x0A
#define SCSI_SENSE_KEY_ABORTED_COMMAND 0x0B
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW 0x0D
#define SCSI_SENSE_KEY_MISCOMPARE 0x0E

#define SCSI_ASENSE_NO_ADDITIONAL_INFORMATION 0x00
#define SCSI_ASENSE_WRITE_FAULT 0x03
#define SCSI_ASENSE_LOGICAL_UNIT_NOT_READY 0x04
#define SCSI_ASENSE_READ_ERROR 0x11
#define SCSI_ASENSE_INVALID_COMMAND 0x20
#define SCSI_ASENSE_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE 0x21
#define SCSI_ASENSE_INVALID_FIELD_IN_CDB 0x24
#define SCSI_ASENSE_WRITE_PROTECTED 0x27
#define SCSI_ASENSE_NOT_READY_TO_READY_CHANGE 0x28
#define SCSI_ASENSE_FORMAT_ERROR 0x31
#define SCSI_ASENSE_MEDIUM_NOT_PRESENT 0x3A

#define SCSI_ASENSEQ_NO_QUALIFIER 0x00
#define SCSI_ASENSEQ_FORMAT_COMMAND_FAILED 0x01
#define SCSI_ASENSEQ_INITIALIZING_COMMAND_REQUIRED 0x02
#define SCSI_ASENSEQ_OPERATION_IN_PROGRESS 0x07

// only support up to 10 byte command
typedef union {
    typedef struct {
        uint8_t  op_code;
        uint8_t  msb;
        uint16_t block_address;
        uint8_t  lenght;
        uint8_t  control;
    } __attribute__((packed)) cdb6_t; // 6byte command descriptor block

    typedef struct {
        uint8_t  op_code;
        uint8_t  msb;
        uint32_t block_address;
        uint8_t  msb1;
        uint16_t lenght;
        uint8_t  control;
    } __attribute__((packed)) cdb10_t; // 10byte command descriptor block
} cdb_t;
