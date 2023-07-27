#include <string.h>
#include "usb_msd.h"
#include "hal_ioblock.h"
#if HAL_USE_MASS_STORAGE_USB || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/
static USBMassStorageDriver *driver_head;

/**
 * @brief The list of Serial USB driver
 */
/* Std setup request
 */
#    define CLEAR_FEATURE 0x01

/* Request types */
#    define MSD_REQ_RESET 0xFF
#    define MSD_GET_MAX_LUN 0xFE

/* CBW/CSW block signatures */
#    define MSD_CBW_SIGNATURE 0x43425355
#    define MSD_CSW_SIGNATURE 0x53425355

/* Setup packet access macros */
#    define MSD_SETUP_WORD(setup, index) (uint16_t)(((uint16_t)setup[index + 1] << 8) | (setup[index] & 0x00FF))
#    define MSD_SETUP_VALUE(setup) MSD_SETUP_WORD(setup, 2)
#    define MSD_SETUP_INDEX(setup) MSD_SETUP_WORD(setup, 4)
#    define MSD_SETUP_LENGTH(setup) MSD_SETUP_WORD(setup, 6)

/* Command statuses */
#    define MSD_COMMAND_PASSED 0x00
#    define MSD_COMMAND_FAILED 0x01
#    define MSD_COMMAND_PHASE_ERROR 0x02

/* SCSI commands */
#    define SCSI_CMD_REQUEST_SENSE 0x03
#    define SCSI_CMD_TEST_UNIT_READY 0x00
#    define SCSI_CMD_FORMAT_UNIT 0x04
#    define SCSI_CMD_INQUIRY 0x12
#    define SCSI_CMD_MODE_SENSE_6 0x1A
#    define SCSI_CMD_START_STOP_UNIT 0x1B
#    define SCSI_CMD_SEND_DIAGNOSTIC 0x1D
#    define SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL 0x1E
#    define SCSI_CMD_READ_FORMAT_CAPACITIES 0x23
#    define SCSI_CMD_READ_CAPACITY_10 0x25
#    define SCSI_CMD_READ_10 0x28
#    define SCSI_CMD_WRITE_10 0x2A
#    define SCSI_CMD_VERIFY_10 0x2F

/* SCSI sense keys */
#    define SCSI_SENSE_KEY_GOOD 0x00
#    define SCSI_SENSE_KEY_RECOVERED_ERROR 0x01
#    define SCSI_SENSE_KEY_NOT_READY 0x02
#    define SCSI_SENSE_KEY_MEDIUM_ERROR 0x03
#    define SCSI_SENSE_KEY_HARDWARE_ERROR 0x04
#    define SCSI_SENSE_KEY_ILLEGAL_REQUEST 0x05
#    define SCSI_SENSE_KEY_UNIT_ATTENTION 0x06
#    define SCSI_SENSE_KEY_DATA_PROTECT 0x07
#    define SCSI_SENSE_KEY_BLANK_CHECK 0x08
#    define SCSI_SENSE_KEY_VENDOR_SPECIFIC 0x09
#    define SCSI_SENSE_KEY_COPY_ABORTED 0x0A
#    define SCSI_SENSE_KEY_ABORTED_COMMAND 0x0B
#    define SCSI_SENSE_KEY_VOLUME_OVERFLOW 0x0D
#    define SCSI_SENSE_KEY_MISCOMPARE 0x0E

#    define SCSI_ASENSE_NO_ADDITIONAL_INFORMATION 0x00
#    define SCSI_ASENSE_WRITE_FAULT 0x03
#    define SCSI_ASENSE_LOGICAL_UNIT_NOT_READY 0x04
#    define SCSI_ASENSE_READ_ERROR 0x11
#    define SCSI_ASENSE_INVALID_COMMAND 0x20
#    define SCSI_ASENSE_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE 0x21
#    define SCSI_ASENSE_INVALID_FIELD_IN_CDB 0x24
#    define SCSI_ASENSE_WRITE_PROTECTED 0x27
#    define SCSI_ASENSE_NOT_READY_TO_READY_CHANGE 0x28
#    define SCSI_ASENSE_FORMAT_ERROR 0x31
#    define SCSI_ASENSE_MEDIUM_NOT_PRESENT 0x3A

#    define SCSI_ASENSEQ_NO_QUALIFIER 0x00
#    define SCSI_ASENSEQ_FORMAT_COMMAND_FAILED 0x01
#    define SCSI_ASENSEQ_INITIALIZING_COMMAND_REQUIRED 0x02
#    define SCSI_ASENSEQ_OPERATION_IN_PROGRESS 0x07

/**
 * @brief Response to a READ_CAPACITY_10 SCSI command
 */
typedef struct {
    uint32_t last_block_addr;
    uint32_t block_size;
} PACK_VAR msd_scsi_read_capacity_10_response_t;

/**
 * @brief Response to a READ_FORMAT_CAPACITIES SCSI command
 */
typedef struct {
    uint8_t  reserved[3];
    uint8_t  capacity_list_length;
    uint32_t block_count;
    uint32_t desc_and_block_length;
} PACK_VAR msd_scsi_read_format_capacities_response_t;

/**
 * @brief   Read-write buffers (TODO: need a way of specifying the size of this)
 */
static uint8_t rw_buf[2][512];

/**
 * @brief Byte-swap a 32 bits unsigned integer
 */
#    define swap_uint32(x) ((((x)&0x000000FF) << 24) | (((x)&0x0000FF00) << 8) | (((x)&0x00FF0000) >> 8) | (((x)&0xFF000000) >> 24))

/**
 * @brief Byte-swap a 16 bits unsigned integer
 */
#    define swap_uint16(x) ((((x)&0x00FF) << 8) | (((x)&0xFF00) >> 8))

static USBOutEndpointState ms_out_ep_state;
static USBInEndpointState  ms_in_ep_state;

// forward declaration
static void msd_ep_notification(USBDriver *usbp, usbep_t ep) {
    (void)usbp;
    chSysLockFromISR();
    chBSemSignalI(&((USBMassStorageDriver *)usbp->in_params[ep])->bsem);
    chSysUnlockFromISR();
}

static const USBEndpointConfig ms_ep_config = {
    USB_EP_MODE_TYPE_BULK, /* Bulk EP */
    NULL,                  /* SETUP packet notification callback */
    &msd_ep_notification,  /* IN notification callback */
    &msd_ep_notification,  /* OUT notification callback */
    MS_EPSIZE,             /* IN maximum packet size */
    MS_EPSIZE,             /* OUT maximum packet size */
    &ms_in_ep_state,       /* IN Endpoint state */
    &ms_out_ep_state,      /* OUT endpoint state */
};

/**
 * @brief   USB device configured handler.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @iclass
 */
void msdConfigureHookI(USBDriver *usbp) {
    (void)usbp;
    USBMassStorageDriver *msdp = driver_head;
    usbInitEndpointI(usbp, msdp->config->bulk_ep_idx, &ms_ep_config);
    msdp->state = MSD_RESET;
    chBSemSignalI(&msdp->bsem);
}

void msdSuspendHookI(USBDriver *usbp) {
    (void)usbp;
    USBMassStorageDriver *msdp = driver_head;
    msdp->state                = MSD_RESET;
    msdp->eject_requested      = true;
    chBSemSignalI(&msdp->bsem);
}

/**
 * @brief   Default requests hook.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @return              The hook status.
 * @retval TRUE         Message handled internally.
 * @retval FALSE        Message not handled.
 */
bool_t msdRequestsHook(USBDriver *usbp) {
    /* usbp->setup fields:
     *  0:   bmRequestType (bitmask)
     *  1:   bRequest
     *  2,3: (LSB,MSB) wValue
     *  4,5: (LSB,MSB) wIndex
     *  6,7: (LSB,MSB) wLength (number of bytes to transfer if there is a data phase) */
    /* check that the request is of type Class / Interface */

    if (((usbp->setup[0] & USB_RTYPE_TYPE_MASK) == USB_RTYPE_TYPE_CLASS) && ((usbp->setup[0] & USB_RTYPE_RECIPIENT_MASK) == USB_RTYPE_RECIPIENT_INTERFACE)) {
        USBMassStorageDriver *msdp = driver_head;

        // uint8_t               index = usbp->setup[4]; // TODO: wIndex is actually uint16_t in LE

        /* act depending on bRequest = setup[1] */
        switch (usbp->setup[1]) {
            // Bulk-Only Mass Storage Reset
            case MSD_REQ_RESET:
                /* check that it is a HOST2DEV request */
                if (((usbp->setup[0] & USB_RTYPE_DIR_MASK) != USB_RTYPE_DIR_HOST2DEV) || (MSD_SETUP_LENGTH(usbp->setup) != 0) || (MSD_SETUP_VALUE(usbp->setup) != 0)) {
                    return FALSE;
                }
                /*
                            As required by the BOT specification, the Bulk-only mass storage reset request (classspecific
                            request) is implemented. This request is used to reset the mass storage device and
                            its associated interface. This class-specific request should prepare the device for the next
                            CBW from the host.
                            To generate the BOT Mass Storage Reset, the host must send a device request on the
                            default pipe of:
                            • bmRequestType: Class, interface, host to device
                            • bRequest field set to 255 (FFh)
                            • wValue field set to ‘0’
                            • wIndex field set to the interface number
                            • wLength field set to ‘0’
                */

                /* The device shall NAK the status stage of the device request until
                 * the Bulk-Only Mass Storage Reset is complete.
                 */
                chSysLockFromISR();
                chBSemResetI(&(msdp->bsem), false);
                usbStallReceiveI(usbp, msdp->config->bulk_ep_idx);
                usbStallTransmitI(usbp, msdp->config->bulk_ep_idx);
                chSysUnlockFromISR();
                /* NAK */
                usbSetupTransfer(usbp, 0, 0, NULL);
                msdp->state = MSD_IDLE;
                return TRUE;

            case MSD_GET_MAX_LUN:
                /* check that it is a DEV2HOST request */
                if (((usbp->setup[0] & USB_RTYPE_DIR_MASK) != USB_RTYPE_DIR_DEV2HOST) || (MSD_SETUP_LENGTH(usbp->setup) != 1) || (MSD_SETUP_VALUE(usbp->setup) != 0)) {
                    return FALSE;
                }

                static uint8_t len_buf[1] = {0};
                /* stall to indicate that we don't support LUN */
                usbSetupTransfer(usbp, len_buf, sizeof(len_buf), NULL);
                return TRUE;
            default:
                /* other standard request will be pass to default implementation*/
                return FALSE;
                break;
        }
    }

    return FALSE;
}

/**
 * @brief Wait until the end-point interrupt handler has been called
 */
static bool_t msd_wait_for_isr(USBMassStorageDriver *msdp) {
    /* sleep until it completes */
    chSysLock();
    chBSemWaitS(&msdp->bsem);
    uint8_t state = msdp->state;
    chSysUnlock();
    return state != MSD_RESET;
}

static void msd_eject(USBMassStorageDriver *msdp) {
    msdp->eject_requested = FALSE;
    msdp->bbdp            = NULL;
    chEvtBroadcast(&msdp->evt_ejected);
}

/**
 * @brief Starts sending data
 */
static void msd_start_transmit(USBMassStorageDriver *msdp, const uint8_t *buffer, size_t size) {
    chSysLock();
    usbStartTransmitI(msdp->config->usbp, msdp->config->bulk_ep_idx, buffer, size);
    chSysUnlock();
}

/**
 * @brief Starts receiving data
 */
static void msd_start_receive(USBMassStorageDriver *msdp, uint8_t *buffer, size_t size) {
    chSysLock();
    usbStartReceiveI(msdp->config->usbp, msdp->config->bulk_ep_idx, buffer, size);
    chSysUnlock();
}

/**
 * @brief Changes the SCSI sense information
 */
static inline void msd_scsi_set_sense(USBMassStorageDriver *msdp, uint8_t key, uint8_t acode, uint8_t aqual) {
    msdp->sense.byte[2]  = key;
    msdp->sense.byte[12] = acode;
    msdp->sense.byte[13] = aqual;
}

static bool_t msd_is_ready(USBMassStorageDriver *msdp) {
    BaseBlockDevice *bbdp = msdp->bbdp;
    if (bbdp != NULL && blkIsInserted(bbdp) && blkGetDriverState(bbdp) == BLK_READY) {
        /* device inserted and ready */
        return TRUE;
    } else {
        /* device not present or not ready */
        msd_scsi_set_sense(msdp, SCSI_SENSE_KEY_NOT_READY, SCSI_ASENSE_MEDIUM_NOT_PRESENT, SCSI_ASENSEQ_NO_QUALIFIER);

        if (msdp->bbdp != NULL) msd_eject(msdp);
        return FALSE;
    }
}

/**
 * @brief Processes an INQUIRY SCSI command
 */
static bool_t msd_scsi_process_inquiry(USBMassStorageDriver *msdp) {
    msd_cbw_t *cbw = &(msdp->cbw);

    /* check the EVPD bit (Vital Product Data) */
    if (cbw->scsi_cmd_data[1] & 0x01) {
        /* check the Page Code byte to know the type of product data to reply */
        switch (cbw->scsi_cmd_data[2]) {
            /* unit serial number */
            case 0x80: {
                uint8_t response[] = {'0'}; /* TODO */
                msd_start_transmit(msdp, response, sizeof(response));
                return msd_wait_for_isr(msdp);
            }

            /* unhandled */
            default:
                msd_scsi_set_sense(msdp, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASENSE_INVALID_FIELD_IN_CDB, SCSI_ASENSEQ_NO_QUALIFIER);
                return FALSE;
        }
    } else {
        msd_start_transmit(msdp, (const uint8_t *)&msdp->inquiry, sizeof(msdp->inquiry));
        return msd_wait_for_isr(msdp);
    }
}

/**
 * @brief Processes a REQUEST_SENSE SCSI command
 */
static bool_t msd_scsi_process_request_sense(USBMassStorageDriver *msdp) {
    msd_start_transmit(msdp, (const uint8_t *)&msdp->sense, sizeof(msdp->sense));
    return msd_wait_for_isr(msdp);
}

/**
 * @brief Processes a READ_CAPACITY_10 SCSI command
 */
static bool_t msd_scsi_process_read_capacity_10(USBMassStorageDriver *msdp) {
    if (!msd_is_ready(msdp)) return FALSE;

    static msd_scsi_read_capacity_10_response_t response;

    response.block_size      = swap_uint32(msdp->block_dev_info.blk_size);
    response.last_block_addr = swap_uint32(msdp->block_dev_info.blk_num - 1);

    msd_start_transmit(msdp, (const uint8_t *)&response, sizeof(response));
    return msd_wait_for_isr(msdp);
}

/**
 * @brief Processes a SEND_DIAGNOSTIC SCSI command
 */
static bool_t msd_scsi_process_send_diagnostic(USBMassStorageDriver *msdp) {
    msd_cbw_t *cbw = &(msdp->cbw);

    if (!(cbw->scsi_cmd_data[1] & (1 << 2))) {
        /* only self-test supported - update SENSE key and fail the command */
        msd_scsi_set_sense(msdp, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASENSE_INVALID_FIELD_IN_CDB, SCSI_ASENSEQ_NO_QUALIFIER);
        return FALSE;
    }

    /* TODO: actually perform the test */
    return TRUE;
}

static bool_t msd_do_write(USBMassStorageDriver *msdp, uint32_t start, uint16_t total) {
    /* get the first packet */

    for (uint16_t i = 0; i < total; i++) {
        msd_start_receive(msdp, rw_buf[i % 2], msdp->block_dev_info.blk_size);
        msd_wait_for_isr(msdp);
        if (!blkWrite(msdp->bbdp, start + i, rw_buf[i % 2], 1)) {
            msd_scsi_set_sense(msdp, SCSI_SENSE_KEY_MEDIUM_ERROR, SCSI_ASENSE_WRITE_FAULT, SCSI_ASENSEQ_NO_QUALIFIER);
            return FALSE;
        }
    }
    return TRUE;
}

static bool_t msd_do_read(USBMassStorageDriver *msdp, uint32_t start, uint16_t total) {
    for (uint16_t i = 0; i < total; i++) {
        if (blkRead(msdp->bbdp, start + i, rw_buf[i % 2], 1)) {
            msd_start_transmit(msdp, rw_buf[i % 2], msdp->block_dev_info.blk_size);
            msd_wait_for_isr(msdp);
        } else {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * @brief Processes a READ_WRITE_10 SCSI command
 */
static bool_t msd_scsi_process_start_read_write_10(USBMassStorageDriver *msdp) {
    msd_cbw_t *cbw = &(msdp->cbw);

    if (!msd_is_ready(msdp)) return FALSE;

    if ((cbw->scsi_cmd_data[0] == SCSI_CMD_WRITE_10) && blkIsWriteProtected(msdp->bbdp)) {
        /* device is write protected and a write has been issued */
        /* block address is invalid, update SENSE key and return command fail */
        msd_scsi_set_sense(msdp, SCSI_SENSE_KEY_DATA_PROTECT, SCSI_ASENSE_WRITE_PROTECTED, SCSI_ASENSEQ_NO_QUALIFIER);
        return FALSE;
    }

    uint32_t rw_block_address = swap_uint32(*(uint32_t *)&cbw->scsi_cmd_data[2]);
    uint16_t total            = swap_uint16(*(uint16_t *)&cbw->scsi_cmd_data[7]);

    if (rw_block_address >= msdp->block_dev_info.blk_num) {
        /* block address is invalid, update SENSE key and return command fail */
        msd_scsi_set_sense(msdp, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASENSE_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE, SCSI_ASENSEQ_NO_QUALIFIER);
        return FALSE;
    }

    if (cbw->scsi_cmd_data[0] == SCSI_CMD_WRITE_10) {
        /* process a write command */
        return msd_do_write(msdp, rw_block_address, total);
    } else {
        if (!msd_do_read(msdp, rw_block_address, total)) {
            msd_scsi_set_sense(msdp, SCSI_SENSE_KEY_MEDIUM_ERROR, SCSI_ASENSE_READ_ERROR, SCSI_ASENSEQ_NO_QUALIFIER);
            /* read failed */
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * @brief Processes a START_STOP_UNIT SCSI command
 */
static bool_t msd_scsi_process_start_stop_unit(USBMassStorageDriver *msdp) {
    if ((msdp->cbw.scsi_cmd_data[4] & 0x03) == 0x02) {
        /* Eject */
        msd_eject(msdp);
    }
    return TRUE;
}

/**
 * @brief Processes a MODE_SENSE_6 SCSI command
 */
static bool_t msd_scsi_process_mode_sense_6(USBMassStorageDriver *msdp) {
    static uint8_t response[4] = {
        0x03, /* number of bytes that follow                    */
        0x00, /* medium type is SBC                             */
        0x00, /* not write protected (TODO handle it correctly) */
        0x00  /* no block descrip                          */
    };

    msd_start_transmit(msdp, response, sizeof(response));
    return msd_wait_for_isr(msdp);
}

/**
 * @brief Processes a TEST_UNIT_READY SCSI command
 */
static bool_t msd_scsi_process_test_unit_ready(USBMassStorageDriver *msdp) {
    return msd_is_ready(msdp);
}

/**
 * @brief Waits for a new command block
 */
static void msd_wait_for_command_block(USBMassStorageDriver *msdp) {
    msd_start_receive(msdp, (uint8_t *)&msdp->cbw, sizeof(msdp->cbw));
    msdp->state = MSD_READ_COMMAND_BLOCK;
    msd_wait_for_isr(msdp);
}

/**
 * @brief Reads a newly received command block
 */
// Command Block Wrapper
static void msd_read_command_block(USBMassStorageDriver *msdp) {
    msd_cbw_t *cbw = &(msdp->cbw);

    /* by default transition back to the idle state */
    msdp->state = MSD_IDLE;

    /* check the command */
    if ((cbw->signature != MSD_CBW_SIGNATURE) || (cbw->lun > 0) || ((cbw->data_len > 0) && (cbw->flags & 0x1F)) || (cbw->scsi_cmd_len == 0) || (cbw->scsi_cmd_len > 16)) {
        /* stall both IN and OUT endpoints */
        chSysLock();
        usbStallReceiveI(msdp->config->usbp, msdp->config->bulk_ep_idx);
        usbStallTransmitI(msdp->config->usbp, msdp->config->bulk_ep_idx);
        chSysUnlock();

        return;
    }

    bool_t result;

    /* check the command */
    switch (cbw->scsi_cmd_data[0]) {
        case SCSI_CMD_INQUIRY:
            result = msd_scsi_process_inquiry(msdp);
            break;
        case SCSI_CMD_REQUEST_SENSE:
            result = msd_scsi_process_request_sense(msdp);
            break;
        case SCSI_CMD_READ_CAPACITY_10:
            result = msd_scsi_process_read_capacity_10(msdp);
            break;
        case SCSI_CMD_READ_10:
        case SCSI_CMD_WRITE_10:
            if (msdp->config->rw_activity_callback) msdp->config->rw_activity_callback(TRUE);
            result = msd_scsi_process_start_read_write_10(msdp);
            if (msdp->config->rw_activity_callback) msdp->config->rw_activity_callback(FALSE);
            break;
        case SCSI_CMD_SEND_DIAGNOSTIC:
            result = msd_scsi_process_send_diagnostic(msdp);
            break;
        case SCSI_CMD_MODE_SENSE_6:
            result = msd_scsi_process_mode_sense_6(msdp);
            break;
        case SCSI_CMD_START_STOP_UNIT:
            result = msd_scsi_process_start_stop_unit(msdp);
            break;
        case SCSI_CMD_TEST_UNIT_READY:
            result = msd_scsi_process_test_unit_ready(msdp);
            break;
        case SCSI_CMD_FORMAT_UNIT:
            /* don't handle */
            result = TRUE;
            break;
        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
            /* don't handle */
            result = TRUE;
            break;
        case SCSI_CMD_VERIFY_10:
            /* don't handle */
            result = TRUE;
            break;
        default:
            msd_scsi_set_sense(msdp, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASENSE_INVALID_COMMAND, SCSI_ASENSEQ_NO_QUALIFIER);

            result = FALSE;
            break;
    }

    if (msdp->state == MSD_RESET) return;

    if (result) {
        /* update sense with success status */
        msd_scsi_set_sense(msdp, SCSI_SENSE_KEY_GOOD, SCSI_ASENSE_NO_ADDITIONAL_INFORMATION, SCSI_ASENSEQ_NO_QUALIFIER);

        /* reset data length left */
        cbw->data_len = 0;
    }

    msd_csw_t *csw = &(msdp->csw);

    if (!result && cbw->data_len) {
        /* still bytes left to send, this is too early to send CSW? */
        chSysLock();
        if (cbw->flags & 0x80) // Is data-in?
            // Stall TX-IN
            usbStallTransmitI(msdp->config->usbp, msdp->config->bulk_ep_idx);
        else
            // Stall RX-OUT
            usbStallReceiveI(msdp->config->usbp, msdp->config->bulk_ep_idx);
        chSysUnlock();
    }

    /* update the command status wrapper(csw) and send it to the host */
    csw->status       = result ? MSD_COMMAND_PASSED : MSD_COMMAND_FAILED;
    csw->data_residue = cbw->data_len;
    csw->signature    = MSD_CSW_SIGNATURE;
    csw->tag          = cbw->tag;

    msd_start_transmit(msdp, (const uint8_t *)csw, sizeof(*csw));
    msd_wait_for_isr(msdp);
}

/**
 * @brief Mass storage thread that processes commands
 *
 **/

static THD_WORKING_AREA(mass_storage_thread_wa, 128);
static THD_FUNCTION(mass_storage_thread, arg) {
    USBMassStorageDriver *msdp = (USBMassStorageDriver *)arg;
    chRegSetThreadName("USB-MSD");
    while (!chThdShouldTerminateX()) {
        // wait on data depending on the current state
        switch (msdp->state) {
            case MSD_IDLE:
                msd_wait_for_command_block(msdp);
                break;
            case MSD_READ_COMMAND_BLOCK:
                msd_read_command_block(msdp);
                break;
            case MSD_RESET:
                if (msdp->eject_requested) msd_eject(msdp);
                msdp->state = MSD_IDLE;
                break;
        }
    }
}

/**
 * @brief Initializse a USB mass storage driver
 */
void msdInit(USBMassStorageDriver *msdp) {
    chDbgCheck(msdp != NULL);
    USBMassStorageDriver *dp = driver_head;

    if (dp == NULL) {
        driver_head = msdp;
    }

    msdp->config = NULL;
    msdp->thread = NULL;
    msdp->state  = MSD_IDLE;

    /* initialize the driver events */
    chEvtObjectInit(&msdp->evt_ejected);

    /* initialise the binary semaphore as taken */
    chBSemObjectInit(&msdp->bsem, TRUE);

    /* initialise the sense data structure */
    size_t i;
    for (i = 0; i < sizeof(msdp->sense.byte); i++)
        msdp->sense.byte[i] = 0x00;
    msdp->sense.byte[0] = 0x70; /* response code */
    msdp->sense.byte[7] = 0x0A; /* additional sense length */

    /* initialize the inquiry data structure */
    msdp->inquiry.peripheral           = 0x00; /* direct access block device  */
    msdp->inquiry.removable            = 0x80; /* removable                   */
    msdp->inquiry.version              = 0x04; /* SPC-2                       */
    msdp->inquiry.response_data_format = 0x02; /* response data format        */
    msdp->inquiry.additional_length    = 0x20; /* response has 0x20 + 4 bytes */
    msdp->inquiry.sccstp               = 0x00;
    msdp->inquiry.bqueetc              = 0x00;
    msdp->inquiry.cmdque               = 0x00;
}

/**
 * @brief Starts a USB mass storage driver
 */
void msdStart(USBMassStorageDriver *msdp, const USBMassStorageConfig *config) {
    chDbgCheck(msdp != NULL);
    chDbgCheck(config != NULL);
    chDbgCheck(msdp->thread == NULL);

    config->usbp->in_params[config->bulk_ep_idx]  = (void *)msdp;
    config->usbp->out_params[config->bulk_ep_idx] = (void *)msdp;

    /* save the configuration */
    msdp->config = config;
    /* set the initial state */
    msdp->state = MSD_IDLE;
    // run the thread
    msdp->thread = chThdCreateStatic(mass_storage_thread_wa, sizeof(mass_storage_thread_wa), NORMALPRIO + 2, /* Initial priority.    */
                                     mass_storage_thread,                                                    /* Thread function.     */
                                     msdp);                                                                  /* Thread parameter.    */

    /* copy the config strings to the inquiry response structure */
    size_t i;
    for (i = 0; i < sizeof(msdp->config->short_vendor_id); ++i)
        msdp->inquiry.vendor_id[i] = config->short_vendor_id[i];
    for (i = 0; i < sizeof(msdp->config->short_product_id); ++i)
        msdp->inquiry.product_id[i] = config->short_product_id[i];
    for (i = 0; i < sizeof(msdp->config->short_product_version); ++i)
        msdp->inquiry.product_rev[i] = config->short_product_version[i];

    if (msdp->thread == NULL) chSysHalt("out of memory");
}

void msdReady(USBMassStorageDriver *msdp, BaseBlockDevice *bbdp) {
    if (msdp->bbdp != NULL) return;
    /* get block device information */
    blkGetInfo(bbdp, &msdp->block_dev_info);
    msdp->bbdp = bbdp;
}

/**
 * @brief   Notify the host that media is ejected
 */
void msdEject(USBMassStorageDriver *msdp) {
    msdp->eject_requested = TRUE;
}

/**
 * @brief Stops a USB mass storage driver
 */
void msdStop(USBMassStorageDriver *msdp) {
    chDbgCheck(msdp->thread != NULL);

    /* notify the thread that it's over */
    chThdTerminate(msdp->thread);

    /* wake the thread up and wait until it ends */
    chSysLock();
    msdp->state = MSD_RESET;
    chBSemSignalI(&msdp->bsem);
    chSysUnlock();
    chThdWait(msdp->thread);
    msdp->thread = NULL;
}

#endif // HAL_USE_MASS_STORAGE_USB
