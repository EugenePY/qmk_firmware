/* Debugger Printf, since will reset as a msc the default debug console will be
 * disconnected, use the swo for debugging, however some cortex M didn't support
 * the swo.
 */
#pragma once
#include "sendchar.h"

#ifdef DAPLINK_DEBUG

#    ifdef CONSOLE_ENABLE
#        error "The console needs to be disaabled in the makefile to use the DAPLINK swv feature"
#    endif

// Cortex M4 Debug Exception and Monitor Control Register base address
#    define DEMCR *((volatile uint32_t*)0xE0000E80)
/* ITM register addresses */
#    define ITM_STIMULUS_PORT0 *((volatile uint32_t*)0xE0000000)
#    define ITM_TRACE_EN *((volatile uint32_t*)0xE0000E00)

uint8_t sendchar(uint8_t c) {
    // Enable TRCENA
    DEMCR |= (1 << 24);

    // enable stimulus port 0
    ITM_TRACE_EN |= (1 << 0);

    // read FIFO status in bit [0]:
    while (!(ITM_STIMULUS_PORT0 & 1))
        ;

    // Write to ITM stimulus port0
    ITM_STIMULUS_PORT0 = ch;
    return 0;
}

#else

__attribute__((weak)) uint8_t sendchar(uint8_t c) {
    return 0;
}

#endif
