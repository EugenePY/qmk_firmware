MODULE = GUI
$(MODULE)_SRC += $(wildcard $(PROJECT_LIB_PATH)/gui/*.c)

$(MODULE)_INC += $(PROJECT_LIB_PATH)/gui 
OPT_DEFS += 
SRC += ${GUI_SRC}
VPATH += ${GUI_INC}
