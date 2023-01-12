# MCU name
MCU = STM32F072
PROJECT_PATH = keyboards/ekow/model_oled
# Bootloader selection
BOOTLOADER = stm32-dfu

# Build Options
#   change yes to no to disable
#
DEBUG = no

BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control

ifeq ($(DEBUG), yes)
	CONSOLE_ENABLE = yes # Console for debug
endif

COMMAND_ENABLE = yes        # Commands for debug and configuration
NKRO_ENABLE = yes           # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output
RAW_ENABLE = yes            # for OLED Update Image

SOLENOIDE_ENABLE = yes
OLED_ENABLE = yes

ifeq ($(OLED_ENABLE), yes)
	OLED_DRIVER = custom
	include $(PROJECT_PATH)/ssd1331/oled.mk
endif

ifeq ($(SOLENOIDE_ENABLE), yes)
	OPT_DEFS += -DSOLENOIDE_ENABLE
	SRC += $(PROJECT_PATH)/solenoid.c
endif
