# PETI Makefile
# Version 1.0

#include local_settings.mk

TARGET_EXEC := PETI.out

TOOLS_PATH := /home/patches/ti/ccs1020/ccs/tools/compiler/ti-cgt-msp430_20.2.2.LTS
CCS_BASEPATH := /home/patches/ti/ccs1020/ccs/ccs_base/msp430
CCS_BASEINCPATH := $(CCS_BASEPATH)/include
PROJ_DIR = /home/patches/HardwareProjects/PETI/

# Some Needed Bins
CC		= $(TOOLS_PATH)/bin/cl430
CXX		= $(TOOLS_PATH)/bin/cl430
LD		= $(TOOLS_PATH)/bin/cl430

# Project Vars
BUILD_DIR 	= build
SRC_DIR 	= src

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. Make will incorrectly expand these otherwise.
SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# String substitution for every C/C++ file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.obj)
OBJS := $(OBJS:.c.obj=.obj)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.pp
DEPS := $(OBJS:.obj=.pp)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIR) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -i,$(INC_DIRS))

# -ppd here should create our dependency files. I think.
CC_FLAGS := -vmspx --code_model=large --data_model=restricted --near_data=none --use_hw_mpy=F5 -i$(CCS_BASEINCPATH) -i$(TOOLS_PATH)/include  -i$(TOOLS_PATH)/lib -i$(CCS_BASEPATH)/lib/FR59xx -i$(CCS_BASEPATH)/lib/5xx_6xx_FRxx $(INC_FLAGS) --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile -ppd

LDFLAGS := $(CC_FLAGS) -z --priority --reread_libs --define=_MPU_ENABLE --diag_wrap=off --display_error_number --warn_sections --xml_link_info="PETI_linkInfo.xml" --use_hw_mpy=F5 --rom_model -m"PETI.map" --heap_size=160 --stack_size=160 --cinit_hold_wdt=on

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) "lnk_msp430fr5994.cmd" -llibmpu_init.a -llibmath.a  -llibc.a

# Build step for C source
$(BUILD_DIR)/%.obj: %.c
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@ --pp_directory=$(dir $@) --obj_directory=$(dir $@)

echo:
	echo $(OBJS)
	echo $(LDFLAGS)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# Include the .pp makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .pp files will be missing, and we don't want those
# errors to show up.
#-include $(DEPS)