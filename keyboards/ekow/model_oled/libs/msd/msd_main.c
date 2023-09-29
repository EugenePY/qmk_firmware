#include <stdio.h>
#include <string.h>

#include <stdio.h>

#include <ch.h>
#include <hal.h>
#include "quantum.h"

#include "usb_msd.h"
#include "usb_main.h"
#include "flash_ioblock.h"
#include "msd_main.h"

/* endpoint index */
#define MSD_DEVICE_VER 0x0000

#define USB_MS_DATA_EP 1
#define MASS_STORAGE_IN_EPADDR ((USB_MS_DATA_EP) | (0x80))
#define MASS_STORAGE_OUT_EPADDR ((USB_MS_DATA_EP) | (0x00))

/* USB device descriptor */
static const uint8_t deviceDescriptorData[] = {USB_DESC_DEVICE(0x0200,                            /* supported USB version (2.0)                     */
                                                               0x00,                              /* device class (none, specified in interface)     */
                                                               0x00,                              /* device sub-class (none, specified in interface) */
                                                               0x00,                              /* device protocol (none, specified in interface)  */
                                                               64,                                /* max packet size of control end-point            */
                                                               VENDOR_ID, PRODUCT_ID, DEVICE_VER, /* device release number                           */
                                                               1,                                 /* index of manufacturer string descriptor         */
                                                               2,                                 /* index of product string descriptor              */
                                                               3,                                 /* index of serial number string descriptor        */
                                                               1                                  /* number of possible configurations               */
                                                               )};

static const USBDescriptor deviceDescriptor = {sizeof(deviceDescriptorData), deviceDescriptorData};

/* configuration descriptor */
static const uint8_t configurationDescriptorData[] = {
    /* configuration descriptor */
    USB_DESC_CONFIGURATION(32,   /* total length                                             */
                           1,    /* number of interfaces                                     */
                           1,    /* value that selects this configuration                    */
                           0,    /* index of string descriptor describing this configuration */
                           0xC0, /* attributes (self-powered)                                */
                           250   /* max power (500 mA)                                       */
                           ),

    /* interface descriptor */
    USB_DESC_INTERFACE(0,    /* interface number                                     */
                       0,    /* value used to select alternative setting             */
                       2,    /* number of end-points used by this interface          */
                       0x08, /* interface class (Mass Storage)                       */
                       0x06, /* interface sub-class (SCSI Transparent Storage)       */
                       0x50, /* interface protocol (Bulk Only)                       */
                       0     /* index of string descriptor describing this interface */
                       ),

    /* end-point descriptor */
    USB_DESC_ENDPOINT(MASS_STORAGE_OUT_EPADDR, /* address (end point index | OUT direction)      */
                      USB_EP_MODE_TYPE_BULK,   /* attributes (bulk)                              */
                      MS_EPSIZE,               /* max packet size(8, 16, 32, 64)                 */
                      0x05                     /* polling interval (ignored for bulk end-points) */
                      ),

    /* end-point descriptor */
    USB_DESC_ENDPOINT(MASS_STORAGE_IN_EPADDR, /* address (end point index | IN direction)       */
                      USB_EP_MODE_TYPE_BULK,  /* attributes (bulk)                              */
                      MS_EPSIZE,              /* max packet size                                */
                      0x05                    /* polling interval (ignored for bulk end-points) */
                      ),
    // ADDing Normal HID interface
};
static const USBDescriptor configurationDescriptor = {sizeof(configurationDescriptorData), configurationDescriptorData};

/* Language descriptor */
static const uint8_t languageDescriptorData[] = {
    USB_DESC_BYTE(4), USB_DESC_BYTE(USB_DESCRIPTOR_STRING), USB_DESC_WORD(0x0409) /* U.S. english */
};
static const USBDescriptor languageDescriptor = {sizeof(languageDescriptorData), languageDescriptorData};

/* Vendor descriptor */
static const uint8_t       vendorDescriptorData[] = {USB_DESC_BYTE(30), USB_DESC_BYTE(USB_DESCRIPTOR_STRING), 'P', 0, 'l', 0, 'a', 0, 'y', 0, 'K', 0, 'B', 0, 'x', 0, 'K', '0', 'e', '0', 'e', '0', 'B', '0', 'o', '0', 'y', '0', 'z', '0'};
static const USBDescriptor vendorDescriptor       = {sizeof(vendorDescriptorData), vendorDescriptorData};

/* Product descriptor */
static const uint8_t       productDescriptorData[] = {USB_DESC_BYTE(22), USB_DESC_BYTE(USB_DESCRIPTOR_STRING), 'M', 0, 'O', 0, 'D', 0, 'E', 0, 'L', 0, '-', 0, 'O', 0, 'L', 0, 'E', 0, 'D', 0};
static const USBDescriptor productDescriptor       = {sizeof(productDescriptorData), productDescriptorData};

/* Serial number descriptor */
static const uint8_t       serialNumberDescriptorData[] = {USB_DESC_BYTE(26), USB_DESC_BYTE(USB_DESCRIPTOR_STRING), '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '1', 0};
static const USBDescriptor serialNumberDescriptor       = {sizeof(serialNumberDescriptorData), serialNumberDescriptorData};

/* Handles GET_DESCRIPTOR requests from the USB host */
static const USBDescriptor* getDescriptor(USBDriver* usbp, uint8_t type, uint8_t index, uint16_t lang) {
    (void)usbp;
    (void)lang;

    switch (type) {
        case USB_DESCRIPTOR_DEVICE:
            return &deviceDescriptor;

        case USB_DESCRIPTOR_CONFIGURATION:
            return &configurationDescriptor;

        case USB_DESCRIPTOR_STRING:
            switch (index) {
                case 0:
                    return &languageDescriptor;
                case 1:
                    return &vendorDescriptor;
                case 2:
                    return &productDescriptor;
                case 3:
                    return &serialNumberDescriptor;
            }
    }

    return 0;
}

/* Handles global events of the USB driver */
static void usbEvent(USBDriver* usbp, usbevent_t event) {
    switch (event) {
        case USB_EVENT_CONFIGURED:
            chSysLockFromISR();

            msdConfigureHookI(usbp);

            chSysUnlockFromISR();
            break;

        case USB_EVENT_RESET:
        case USB_EVENT_ADDRESS:
        case USB_EVENT_SUSPEND:
        case USB_EVENT_WAKEUP:
        case USB_EVENT_STALLED:
        default:
            break;
    }
}

/* Configuration of the USB driver */

static USBConfig msd_usbConfig = {usbEvent, getDescriptor, msdRequestsHook, NULL};

/* Turns on a LED when there is I/O activity on the USB port */
static void usbActivity(bool_t active){

};
/* USB mass storage configuration */

const USBMassStorageConfig msdConfig = {&USB_DRIVER, 0, USB_MS_DATA_EP, &usbActivity, "PKBxKBL", "ModelOLED", "0.1"};

/* USB mass storage driver */

/* Flash Block Device*/
FLASHDriver* flash_block_device_ptr;

// Main Functions expose

void msd_protocol_setup(USBMassStorageDriver *umsd) {
    /* Wait until USB is active */
    usbObjectInit(umsd->config->usbp);
    /* initialize the USB mass storage driver */
    msdInit(umsd);

    /* start the USB mass storage service */
    msdStart(umsd, &msdConfig);

    flashInit();
    flash_block_device_ptr = get_flashObject();
    msdReady(umsd, (BaseBlockDevice*)flash_block_device_ptr);

    // start the USB
    usbDisconnectBus(umsd->config->usbp);
    usbStop(umsd->config->usbp);
    usbStart(umsd->config->usbp, &msd_usbConfig);
    usbConnectBus(umsd->config->usbp);

    while (true) {
        if (umsd->config->usbp->state == USB_ACTIVE) {
            break;
        }
        wait_ms(50);
    }
}
