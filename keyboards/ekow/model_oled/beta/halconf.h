#pragma once
//#include "stm32_registry.h"
//#undef STM32_OTG1_ENDPOINTS
//#define STM32_OTG1_ENDPOINTS 6 
// how the qmk count endpoint is different from Chibios 
#define HAL_USE_SPI TRUE
#define SPI_USE_WAIT TRUE
#define SPI_SELECT_MODE SPI_SELECT_MODE_PAD
#define HAL_USE_MASS_STORAGE_USB TRUE

#include_next <halconf.h>
