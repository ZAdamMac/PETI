##
# Foonly's Magic Makefile
##

BASEDIR 	= .
SRCDIR 		= src
DEPLOYDIR 	= deploy
TMPDIR 		= /tmp
VERSION 	= "   0.1.0-tidy   "  # Should always be padded to 16 chars
TARGET 		= PETI_firmware_v(VERSION)
MCU 		= msp430fr5994

all: target size

target-check:
	mspdebug tilib "exit"

target-flash: target
	mspdebug tilib "prog ./bin/firmware.elf"

target-startdebug: target
	skill mspdebug
	mspdebug tilib "gdb"

target-debug:
	make target-startdebug &
	msp430-gdb bin/firmware.elf -ex "target remote :2000"

clean:
	make -C $(SRCDIR) clean
	rm -fr doc/generated
	rm -f bin/firmware.*