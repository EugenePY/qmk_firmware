MODULE = VFS

$(MODULE)_SRC += $(PROJECT_LIB_PATH)/vfs/vfat.c $(PROJECT_LIB_PATH)/vfs/flash.c \
			     $(PROJECT_LIB_PATH)/vfs/flash_ioblock.c \
				
$(MODULE)_INC += $(PROJECT_LIB_PATH)/vfs
$(MODULE)_INC += $(FATFS_INC)


SRC += ${VFS_SRC}
VPATH += ${VFS_INC}
