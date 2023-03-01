fatfs_CONFIG :=  $(PROJECT_PATH)/libs/vfs/fatfs/include/ffconf.h
fatfs_SRC  := $(PROJECT_PATH)/tests/fatfs_test.cpp \
			  $(PROJECT_PATH)/tests/fatfs_mock.cpp \
			  $(PROJECT_PATH)/libs/vfs/fatfs/ff.c

fatfs_INC := $(PROJECT_PATH)/tests $(PROJECT_PATH)/libs/\
			 $(PROJECT_PATH)/libs/vfs/fatfs/include
				
fatfs_DEFS := -DTEST -DFS_FATFS_WINDOW_ALIGNMENT=1


