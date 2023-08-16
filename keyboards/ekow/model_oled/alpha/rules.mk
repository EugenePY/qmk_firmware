# MCU name
MCU = STM32F411
BOARD = GENERIC_STM32_F411XE
MCU_LDSCRIPT = oled

PROJECT_PATH = keyboards/ekow/model_oled
PROJECT_LIB_PATH = $(PROJECT_PATH)/libs

# include the libs path
VPATH +=  $(PROJECT_LIB_PATH)

# Bootloader selection
BOOTLOADER = stm32-dfu

# Build Options
#   change yes to no to disable
BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
NKRO_ENABLE = yes           # Enable N-Key Rollover
#RAW_ENABLE = yes #this will be enable when using via

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

CUSTOM_MATRIX = yes
# project specific files
SRC += matrix.c

