MODULE = VFS

FATFS_PATH = $(PROJECT_LIB_PATH)/vfs/fatfs

FATFS_SRC = $(filter-out $(FATFS_PATH)/diskio.c,$(wildcard $(FATFS_PATH)/*.c))

FATFS_INC = $(FATFS_PATH)/include


$(MODULE)_SRC += $(PROJECT_LIB_PATH)/vfs/vfat.c $(PROJECT_LIB_PATH)/vfs/flash.c \
			     $(PROJECT_LIB_PATH)/vfs/flash_ioblock.c \
				 $(PROJECT_LIB_PATH)/vfs/diskio_stm32.c
OPT_DEFS += -DFS_FATFS_WINDOW_ALIGNMENT=1
				
$(MODULE)_SRC += $(FATFS_SRC) 
$(MODULE)_INC += $(PROJECT_LIB_PATH)/vfs
$(MODULE)_INC += $(FATFS_INC)


SRC += ${VFS_SRC}
VPATH += ${VFS_INC}
