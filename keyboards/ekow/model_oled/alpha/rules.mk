# MCU name
MCU = STM32F411
BOARD = GENERIC_STM32_F411XE
<<<<<<< Updated upstream

MCU_LDSCRIPT = oled

PROJECT_PATH = keyboards/ekow/model_oled
PROJECT_LIB_PATH = $(PROJECT_PATH)/libs

# include the libs path
VPATH +=  $(PROJECT_LIB_PATH)

# Bootloader selection
BOOTLOADER = stm32-dfu

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
NKRO_ENABLE = yes           # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output

SOLENOIDE_ENABLE = yes
OLED_ENABLE = yes

ifeq ($(OLED_ENABLE), yes)
	OLED_DRIVER = custom
	include $(PROJECT_LIB_PATH)/oled.mk
	# image src 
	SRC += $(PROJECT_PATH)/img/icon.c
endif

ifeq ($(SOLENOIDE_ENABLE), yes)
	HAPTIC_ENABLE = yes
	HAPTIC_DRIVER += SOLENOID
endif
=======
>>>>>>> Stashed changes

CUSTOM_MATRIX = yes
ifeq ($(CUSTOM_MATRIX), yes)
	SRC += matrix.c
endif
