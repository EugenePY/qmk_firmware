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

MODEL_OLED_QUANTUM_PAINTER_ENABLE = yes
ifeq ($(MODEL_OLED_QUANTUM_PAINTER_ENABLE), yes)
	QUANTUM_PAINTER_DRIVERS += ssd1331_spi
	QUANTUM_PAINTER_ANIMATIONS_ENABLE = yes
	QUANTUM_PAINTER_NEEDS_COMMS_SPI = yes
	QUANTUM_PAINTER_NEEDS_COMMS_SPI_DC_RESET = yes

	include $(PROJECT_LIB_PATH)/oled.mk
	include $(PROJECT_LIB_PATH)/painter/painter.mk
	# default animation...
	SRC += $(wildcard $(PROJECT_PATH)/img/*.c)
	VPATH += $(PROJECT_PATH)/img
endif

ifeq ($(SOLENOIDE_ENABLE), yes)
	HAPTIC_ENABLE = yes
	HAPTIC_DRIVER += SOLENOID
endif

SRC += graphic.c
