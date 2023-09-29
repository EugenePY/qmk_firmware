# ssd1331 painter support

OPT_DEFS += -DQUANTUM_PAINTER_ENABLE
COMMON_VPATH += $(QUANTUM_DIR)/painter \
                $(QUANTUM_DIR)/unicode
SRC += \
    $(QUANTUM_DIR)/unicode/utf8.c \
    $(QUANTUM_DIR)/color.c \
    $(QUANTUM_DIR)/painter/qp.c \
    $(QUANTUM_DIR)/painter/qp_stream.c \
    $(QUANTUM_DIR)/painter/qgf.c \
    $(QUANTUM_DIR)/painter/qff.c \
    $(QUANTUM_DIR)/painter/qp_draw_core.c \
    $(QUANTUM_DIR)/painter/qp_draw_codec.c \
    $(QUANTUM_DIR)/painter/qp_draw_circle.c \
    $(QUANTUM_DIR)/painter/qp_draw_ellipse.c \
    $(QUANTUM_DIR)/painter/qp_draw_image.c \
    $(QUANTUM_DIR)/painter/qp_draw_text.c \
	$(QUANTUM_DIR)/painter/qp_internal.c


# Check if people want animations... enable the defered exec if so.
ifeq ($(strip $(QUANTUM_PAINTER_ANIMATIONS_ENABLE)), yes)
    DEFERRED_EXEC_ENABLE := yes
    OPT_DEFS += -DQUANTUM_PAINTER_ANIMATIONS_ENABLE
endif

# Comms flags
QUANTUM_PAINTER_NEEDS_COMMS_SPI ?= no

QUANTUM_PAINTER_NEEDS_COMMS_SPI_DC_RESET := yes
OPT_DEFS += -DQUANTUM_PAINTER_SSD1331_ENABLE -DQUANTUM_PAINTER_SSD1331_SPI_ENABLE

COMMON_VPATH += \
	$(DRIVER_PATH)/painter/tft_panel \
	$(PROJECT_LIB_PATH)/painter
SRC += \
	$(DRIVER_PATH)/painter/tft_panel/qp_tft_panel.c \
	$(PROJECT_LIB_PATH)/painter/qp_ssd1331.c

# Iterate through the listed drivers for the build, including what's necessary
$(foreach qp_driver,$(QUANTUM_PAINTER_DRIVERS),$(eval $(call handle_quantum_painter_driver,$(qp_driver))))

# If SPI comms is needed, set up the required files
ifeq ($(strip $(QUANTUM_PAINTER_NEEDS_COMMS_SPI)), yes)
    OPT_DEFS += -DQUANTUM_PAINTER_SPI_ENABLE
    QUANTUM_LIB_SRC += spi_master.c
    VPATH += $(DRIVER_PATH)/painter/comms
    SRC += \
        $(QUANTUM_DIR)/painter/qp_comms.c \
        $(DRIVER_PATH)/painter/comms/qp_comms_spi.c

    ifeq ($(strip $(QUANTUM_PAINTER_NEEDS_COMMS_SPI_DC_RESET)), yes)
        OPT_DEFS += -DQUANTUM_PAINTER_SPI_DC_RESET_ENABLE
    endif
endif

include $(PROJECT_LIB_PATH)/painter/ssd1331.mk

