SILENT=:

PROJECT_PATH = keyboards/ekow/model_oled

.DEFAULT_GOAL := all

OPT = g
TEST = fatfs

include paths.mk
include $(BUILDDEFS_PATH)/message.mk
TEST_PATH = keyboards/ekow/model_oled/tests

GTEST_OUTPUT = $(BUILD_DIR)/gtest

TEST_OBJ = $(BUILD_DIR)/$(TEST)_obj
TARGET=test/$(TEST)



OUTPUTS := $(TEST_OBJ)/$(TEST) $(GTEST_OUTPUT)
GTEST_INC := \
	$(LIB_PATH)/googletest/googletest/include \
	$(LIB_PATH)/googletest/googlemock/include

GTEST_INTERNAL_INC := \
	$(LIB_PATH)/googletest/googletest \
	$(LIB_PATH)/googletest/googlemock

$(GTEST_OUTPUT)_SRC := \
	googletest/src/gtest-all.cc\
	googlemock/src/gmock-all.cc

$(GTEST_OUTPUT)_DEFS :=
$(GTEST_OUTPUT)_INC := $(GTEST_INC) $(GTEST_INTERNAL_INC)

LDFLAGS += -lstdc++ -lpthread -shared-libgcc

CREATE_MAP := no

VPATH += \
	$(LIB_PATH)/googletest \
	$(LIB_PATH)/googlemock \
	$(COMMON_VPATH) \
	$(TEST_PATH)

all: elf


VPATH += $(COMMON_VPATH)
PLATFORM:=TEST
PLATFORM_KEY:=test
BOOTLOADER_TYPE:=none


include $(BUILDDEFS_PATH)/common_features.mk
include $(BUILDDEFS_PATH)/generic_features.mk
include $(PLATFORM_PATH)/common.mk
include $(TMK_PATH)/protocol.mk
include $(QUANTUM_PATH)/logging/print.mk
include $(PROJECT_PATH)/tests/rules.mk

$(TEST)_SRC += \
	tests/test_common/main.cpp \
	$(PLATFORM_COMMON_DIR)/hardware_id.c \
	$(PLATFORM_COMMON_DIR)/platform.c \
	$(PLATFORM_COMMON_DIR)/suspend.c \
	$(PLATFORM_COMMON_DIR)/timer.c \
	$(PLATFORM_COMMON_DIR)/bootloaders/$(BOOTLOADER_TYPE).c \
	$(QUANTUM_PATH)/logging/print.c

# Search Path
VPATH += $(PLATFORM_PATH)
VPATH += $(PLATFORM_PATH)/$(PLATFORM_KEY)
VPATH += $(PLATFORM_PATH)/$(PLATFORM_KEY)/$(DRIVER_DIR)

$(TEST)_INC += tests/test_common/common_config.h

$(TEST)_DEFS += $(TMK_COMMON_DEFS) $(OPT_DEFS) "-DKEYMAP_C=\"keymap.c\""
$(TEST)_CONFIG += $(TEST_PATH)/config.h


$(TEST_OBJ)/$(TEST)_SRC := 	 $($(TEST)_SRC)
$(TEST_OBJ)/$(TEST)_INC := $($(TEST)_INC) $(VPATH) $(GTEST_INC)
$(TEST_OBJ)/$(TEST)_DEFS := $($(TEST)_DEFS)
$(TEST_OBJ)/$(TEST)_CONFIG := $($(TEST)_CONFIG)


include $(PLATFORM_PATH)/$(PLATFORM_KEY)/platform.mk
include $(BUILDDEFS_PATH)/common_rules.mk


$(shell mkdir -p $(BUILD_DIR)/test 2>/dev/null)
$(shell mkdir -p $(TEST_OBJ) 2>/dev/null)

test: $(BUILD_DIR)/$(TARGET).elf
	./$(BUILD_DIR)/$(TARGET).elf

.PHONY: test

gdb:
	# Build qmk 
	qmk --verbose compile -kb ekow/model_oled/alpha -km via
	# openocd flash
	openocd -f ./keyboards/ekow/model_oled/tests/oled.cfg  -c  "program_device ()"

gdb-beta:
	# Build qmk 
	qmk --verbose compile -kb ekow/model_oled/beta -km via
	# openocd flash
	openocd -f ./keyboards/ekow/model_oled/tests/oled-beta.cfg  -c  "program_device ()"



