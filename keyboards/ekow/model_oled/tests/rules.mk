ssd1331_CONFIG :=  $(PROJECT_PATH)/tests/config_mock.h
ssd1331_SRC  := $(PROJECT_PATH)/tests/ssd1331_test.cpp \
				$(PROJECT_PATH)/libs/ssd1331/ssd1331.c \
				$(PROJECT_PATH)/tests/ssd1331_mock.cpp \
				$(PROJECT_PATH)/tests/data/icon.c 

ssd1331_INC := $(PROJECT_PATH)/tests $(PROJECT_PATH)/libs/ssd1331 \
			   $(DRIVER_PATH)/oled 
				
ssd1331_DEFS := -DOLED_TEST
