# MCU name
MCU = STM32F411
BOARD = BLACKPILL_STM32_F411

PROJECT_PATH = keyboards/ekow/model_oled
PROJECT_LIB_PATH = $(PROJECT_PATH)/libs

# include the libs path
VPATH +=  $(PROJECT_LIB_PATH)

# Bootloader selection
BOOTLOADER = stm32-dfu

# Build Options
#   change yes to no to disable
#
DEBUG = yes

BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control

ifeq ($(DEBUG), yes)
	DEBUG_ENABLE = yes
	CONSOLE_ENABLE = yes
endif

COMMAND_ENABLE = yes        # Commands for debug and configuration
NKRO_ENABLE = yes           # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output

SOLENOIDE_ENABLE = yes
OLED_ENABLE = yes

ifeq ($(OLED_ENABLE), yes)
	OLED_DRIVER = custom
	EEPROM_DRIVER = vendor
	include $(PROJECT_LIB_PATH)/oled.mk
endif

ifeq ($(SOLENOIDE_ENABLE), yes)
	HAPTIC_ENABLE = yes
	HAPTIC_DRIVER += SOLENOID
endif
