/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
 * STM32F411xE memory setup.
 */
MEMORY
{
    flash0 (rx) : org = 0x08000000, len = 16k        /* Sector 0    - Init code as ROM bootloader assumes application starts here */
    flash1 (rx) : org = 0x08004000, len = 16k        /* Sector 1    - Emulated eeprom */
    flash2 (rx) : org = 0x08008000, len = 512k - 32k - 384k /* Sector 2..7 - Rest of firmware */
    flash3 (rx) : org = 0x00000000, len = 0
    flash4 (rx) : org = 0x00000000, len = 0
<<<<<<< HEAD:keyboards/ekow/model_oled/alpha/ld/oled.ld
    flash5 (rx) : org = 0x08020000, len = 128k
    flash6 (rx) : org = 0x08040000, len = 128k
=======
<<<<<<< Updated upstream:keyboards/ekow/model_oled/alpha/ld/oled.ld
    flash5 (rx) : org = 0x00000000, len = 0
    flash6 (rx) : org = 0x00000000, len = 0
>>>>>>> model-m-painter:keyboards/ekow/model_oled/ld/oled.ld
    flash7 (rx) : org = 0x08060000, len = 128k
=======
    flash5 (rx) : org = 0x08020000, len = 384k
    flash6 (rx) : org = 0x08040000, len = 0
    flash7 (rx) : org = 0x08060000, len = 0
>>>>>>> Stashed changes:keyboards/ekow/model_oled/ld/oled.ld
    ram0   (wx) : org = 0x20000000, len = 128k
    ram2   (wx) : org = 0x00000000, len = 0
    ram1   (wx) : org = 0x00000000, len = 0
    ram3   (wx) : org = 0x00000000, len = 0
    ram4   (wx) : org = 0x00000000, len = 0
    ram5   (wx) : org = 0x00000000, len = 0
    ram6   (wx) : org = 0x00000000, len = 0
    ram7   (wx) : org = 0x00000000, len = 0
}

/* For each data/text section two region are defined, a virtual region
   and a load region (_LMA suffix).*/

/* Flash region to be used for exception vectors.*/
REGION_ALIAS("VECTORS_FLASH", flash0);
REGION_ALIAS("VECTORS_FLASH_LMA", flash0);

/* Flash region to be used for constructors and destructors.*/
REGION_ALIAS("XTORS_FLASH", flash2);
REGION_ALIAS("XTORS_FLASH_LMA", flash2);

/* Flash region to be used for code text.*/
REGION_ALIAS("TEXT_FLASH", flash2);
REGION_ALIAS("TEXT_FLASH_LMA", flash2);

/* Flash region to be used for read only data.*/
REGION_ALIAS("RODATA_FLASH", flash2);
REGION_ALIAS("RODATA_FLASH_LMA", flash2);

/* Flash region to be used for various.*/
REGION_ALIAS("VARIOUS_FLASH", flash2);
REGION_ALIAS("VARIOUS_FLASH_LMA", flash2);

/* Flash region to be used for RAM(n) initialization data.*/
REGION_ALIAS("RAM_INIT_FLASH_LMA", flash2);

/* RAM region to be used for Main stack. This stack accommodates the processing
   of all exceptions and interrupts.*/
REGION_ALIAS("MAIN_STACK_RAM", ram0);

/* RAM region to be used for the process stack. This is the stack used by
   the main() function.*/
REGION_ALIAS("PROCESS_STACK_RAM", ram0);

/* RAM region to be used for data segment.*/
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("DATA_RAM_LMA", flash2);

/* RAM region to be used for BSS segment.*/
REGION_ALIAS("BSS_RAM", ram0);

/* RAM region to be used for the default heap.*/
REGION_ALIAS("HEAP_RAM", ram0);

/* define the eeprom base address */
<<<<<<< HEAD:keyboards/ekow/model_oled/alpha/ld/oled.ld
__oled_img_base_address__ = ORIGIN(flash5);
__oled_img_flash_size__ = LENGTH(flash5) +  LENGTH(flash6) + LENGTH(flash7);
=======
<<<<<<< Updated upstream:keyboards/ekow/model_oled/alpha/ld/oled.ld
__oled_img_base_address__ = ORIGIN(flash7);
__oled_img_flash_size__ = LENGTH(flash7); 
>>>>>>> model-m-painter:keyboards/ekow/model_oled/ld/oled.ld
__oled_img_end_address__ = __oled_img_base_address__ + __oled_img_flash_size__;
/* Generic rules inclusion.*/
=======

>>>>>>> Stashed changes:keyboards/ekow/model_oled/ld/oled.ld
INCLUDE rules.ld

SECTIONS {
    .graphic : {
        _sgraphic = .;
        *(.graphic)
        _egraphic = .;
        PROVIDE(__oled_img_base_address__ = _sgraphic);
        PROVIDE(__oled_img_flash_size__ = LENGTH(flash5));
        PROVIDE(__oled_img_end_address__ = __oled_img_base_address__ + __oled_img_flash_size__);
    } >flash5
}


