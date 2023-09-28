#pragma once

<<<<<<< Updated upstream

#define HAL_USE_PAL TRUE
=======
>>>>>>> Stashed changes
#define HAL_USE_SPI TRUE
#define SPI_USE_WAIT TRUE
#define SPI_SELECT_MODE SPI_SELECT_MODE_PAD
#define HAL_USE_MASS_STORAGE_USB TRUE
#include_next <halconf.h>
