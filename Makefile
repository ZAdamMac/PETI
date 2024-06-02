# PETI Makefile
# Version 1.0

#include local_settings.mk
include local_settings.mk

# Some Needed Bins
CC		= $(TOOLS_PATH)/bin/cl430
CXX		= $(TOOLS_PATH)/bin/cl430
LD		= $(TOOLS_PATH)/bin/cl430

# Project Vars
BUILD_DIR 	= build
SRC_DIR 	= src
FONT_DIR	= $(SRC_DIR)/lib/display/fonts

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. Make will incorrectly expand these otherwise.
SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Final all the Fonts we want to transform to headers
FNTS := $(shell find $(SRC_DIR) -name '*.fnt')
HFNTS := $(FNTS:%=$(SRC_DIR)/lib/display/fonts/%.h)
HFNTS := $(HFNTS:.fnt.h=.h)

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
CC_FLAGS := -vmspx --code_model=large  --gen_preprocessor_listing --data_model=restricted --near_data=none --use_hw_mpy=F5 -i$(CCS_BASEINCPATH) -i$(TOOLS_PATH)/include  -i$(TOOLS_PATH)/lib -i$(CCS_BASEPATH)/lib/FR59xx -i$(CCS_BASEPATH)/lib/5xx_6xx_FRxx $(INC_FLAGS) --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile -ppd

LDFLAGS := $(CC_FLAGS) -z --priority --reread_libs --define=_MPU_ENABLE --diag_wrap=off --display_error_number --warn_sections --xml_link_info="PETI_linkInfo.xml" --use_hw_mpy=F5 --rom_model -m"PETI.map" --heap_size=160 --stack_size=160 --cinit_hold_wdt=on

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) "lnk_msp430fr5994.cmd" -llibmpu_init.a -llibmath.a  -llibc.a

# Build step for C source
$(BUILD_DIR)/%.obj: %.c $(HFNTS)
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@ --pp_directory=$(dir $@) --obj_directory=$(dir $@)

$(FONT_DIR)/%.h: %.fnt
	python3 $(HELP_DIR)/fontx2-convert/fontx2-convert.py -i $< -o $(FONT_DIR)

echo:
	echo $(OBJS)
	echo $(LDFLAGS)
	echo $(HFNTS)

clean:
	rm -r $(BUILD_DIR)
	rm -r $(FONT_DIR)/*.h

fonts:
	$(foreach)

target-check: 
	mspdebug tilib "exit"

target-flash: $(BUILD_DIR)/$(TARGET_EXEC) fonts
	mspdebug tilib "prog ./$(BUILD_DIR)/$(TARGET_EXEC)"

target-startdebug: $(BUILD_DIR)/$(TARGET_EXEC)
	skill mspdebug
	mspdebug tilib "gdb"

target-debug:
	make target-startdebug &
	msp430-gdb $(BUILD_DIR)/$(TARGET_EXEC) -ex "target remote :2000"

.PHONY: clean echo target-check target-flash target-startdebug target-debug