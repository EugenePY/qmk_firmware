#pragma once
#include "quantum.h"
#include <stdbool.h>

#ifndef SOLENOID_PIN
#define SOLENOID_PIN B3
#endif

typedef struct {
    bool active;
} solendoid_driver_t;


void solenoid_init(void);
void solenoid_driver_toggle(void);
void solenoid_driver_off(void);
void solenoid_driver_on(void);

void solenoid_toggle(void);
void solenoid_on(void);
void solenoid_off(void);


