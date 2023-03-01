# MCU name
MCU = STM32F072

PROJECT_PATH = keyboards/ekow/model_oled

PROJECT_LIB_PATH = $(PROJECT_PATH)/libs

# include the libs path
VPATH +=  $(PROJECT_LIB_PATH)

# Bootloader selection
BOOTLOADER = stm32-dfu

# Build Options
#   change yes to no to disable
#
DEBUG = no

BOOTMAGIC_ENABLE = no # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control

ifeq ($(DEBUG), yes)
	DEBUG_ENABLE = yes
	CONSOLE_ENABLE = yes
	COMMAND_ENABLE = yes        # Commands for debug and configuration
endif

NKRO_ENABLE = no 			# Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output

SOLENOIDE_ENABLE = no
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
