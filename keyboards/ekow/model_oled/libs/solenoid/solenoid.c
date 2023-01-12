#include "quantum.h"
#include "solenoid.h"
#include <stdbool.h>
#include "wait.h"

static solendoid_driver_t solendoid_driver = {
    .active = false,
};

void solenoid_init(void) {
    setPinOutput(SOLENOID_PIN);
    writePinLow(SOLENOID_PIN);
}

void solenoid_driver_toggle(void) {
    solendoid_driver.active ^= true;
}

void solenoid_toggle(void) {
    if (solendoid_driver.active) {
        togglePin(SOLENOID_PIN);
    }
}

void solenoid_driver_off(void) {
    solenoid_off();
    solendoid_driver.active = false;
}

void solenoid_driver_on(void) {
    solendoid_driver.active = true;
}

void solenoid_on(void) {
    if (solendoid_driver.active) {
        writePinHigh(SOLENOID_PIN);
    }
}

void solenoid_off(void) {
    if (solendoid_driver.active) {
        writePinLow(SOLENOID_PIN);
    }
}
