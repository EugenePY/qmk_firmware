# MCU name
MCU = STM32F411
BOARD = GENERIC_STM32_F411XE

# Bootloader selection
BOOTLOADER = stm32-dfu

# Build Options
# change yes to no to disable
BOOTMAGIC_ENABLE = yes # Virtual DIP switch configuration
MOUSEKEY_ENABLE = yes # Mouse keys
EXTRAKEY_ENABLE = yes # Audio control and System control

CONSOLE_ENABLE = yes # Console for debug
#COMMAND_ENABLE = yes # Commands for debug and configuration
NKRO_ENABLE = yes  

# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend

# RGB Underglow
RGBLIGHT_ENABLE = yes
RGBLIGHT_DRIVER = WS2812
WS2812_DRIVER = pwm

# RGB Matrix
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = IS31FL3741

