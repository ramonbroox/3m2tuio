#ifndef _TS3M_H_
#define _TS3M_H_
#include <stdint.h>

/************* Hardware-related constants *************/

#define TS3M_VENDOR_ID    0x0596
#define TS3M_PRODUCT_ID	0x0502

#define TS3M_X_RESOL 0x7FFF
#define TS3M_Y_RESOL 0x7FFF

#define TS3M_TOUCH_REPORT 0x13

#define TS3M_TOUCHING       0x07
#define TS3M_NOT_TOUCHING   0x04
#define TS3M_INVALID_TOUCH  0x00

#define TOUCH_MAXID 255

/************* Customizable constants *************/

/* Number of touches allowed */
#define TS3M_EVENT_MAX_TOUCHES   20

struct _3M_touch_report {
    uint8_t status;
    uint8_t touch_id;
    uint8_t x_lsb;
    uint8_t x_msb;
    uint8_t y_lsb;
    uint8_t y_msb;
    uint8_t width_lsb;
    uint8_t width_msb;
    uint8_t height_lsb;
    uint8_t height_msb;
};

struct _3M_coordinate_report {
    uint8_t report_id;
    struct _3M_touch_report touchs[6];
    uint8_t actual_count;
    uint8_t not_used1;
    uint8_t not_used2;
};

#endif
