#pragma once
#include <stdint.h>
#include <ch.h>

#define WIDTH_TYPE uint8_t
#define HEIGHT_TYPE uint8_t
#define RENDER_BUFFER_SIZE 256

#define METHOD(obj, method)

#define TRANS_MASK 0xff
// colors

#define PIXEL_TYPE uint16_t

uint8_t getR(PIXEL_TYPE pixel);
uint8_t getG(PIXEL_TYPE piexl);
uint8_t getB(PIXEL_TYPE pixel);

//
typedef struct app_s app_t;
typedef struct event_s event_t;

struct {
    uint16_t event_code;
    uint16_t event_data;
} event_s;

// component interface
//

// screen_driver

typedef struct {
} display_driver_t;

// App
struct {
    display_driver_t* device_pt;     // display driver
    thread_t*         msg_thread_pt; //  main messaging thread
    view_t*           root_view_pt;  // ROOT view
} app_s;
