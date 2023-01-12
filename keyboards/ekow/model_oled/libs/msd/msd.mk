MODULE = MSD
$(MODULE)_SRC += $(PROJECT_LIB_PATH)/msd/mass_storage/usb_msd.c \
				 $(PROJECT_LIB_PATH)/msd/mass_storage/storage.c

$(MODULE)_INC += $(PROJECT_LIB_PATH)/msd \
				 $(PROJECT_LIB_PATH)/msd/mass_storage
OPT_DEFS += 
SRC += ${MSD_SRC}
SRC += $(PROJECT_LIB_PATH)/msd/msd_main.c
VPATH += ${MSD_INC}

include $(PROJECT_LIB_PATH)/vfs/vfs.mk
