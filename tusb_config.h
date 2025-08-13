#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#define CFG_TUSB_MCU     OPT_MCU_RP2040
#define CFG_TUSB_OS      OPT_OS_PICO

#define CFG_TUSB_RHPORT0_MODE   OPT_MODE_HOST

#define CFG_TUH_HUB             1
#define CFG_TUH_HID             4
#define CFG_TUH_DEVICE_MAX      4
#define CFG_TUH_ENUMERATION_BUFSIZE 128
#define CFG_TUH_HID_EPIN_BUFSIZE    64
#define CFG_TUH_HID_EPOUT_BUFSIZE   16

#define CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_ALIGN      __attribute__ ((aligned(4)))

#endif // _TUSB_CONFIG_H_
