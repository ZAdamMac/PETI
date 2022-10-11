##
# Common Settings for the Makefiles
# 
# Zac Adam-MacEwen, zadammac@arcanalabs.ca
##

## compiler details
PREFIX			?= 'msp430-'
MCU				?= 'msp430fr5994'

CC				?= $(PREFIX)gcc
LD				?= $(PREFIX)gcc
OBJCOPY			?= $(PREFIX)objcopy

INCDIR 			+= -I.
CFLAGS 			+= -Os -mmcu=$(MCU) -g -Wall -Wextra $(INCDIR) -std=c99 -g3 -ggdb -gdwarf -2

LIBDIR			=
LIBS 			=
LDFLAGS			+= -mmcu=$(MCU) $(LIBDIR) $(LIBS)

# output directory for compiled binaries
BINDIR			?= ../bin

.SUFFIXES: .elf .bin .hex .srec .list .images
.SECONDEXPANSION:
.SECONDARY:

all: images

images: $(BINARY).images

%.images: %.bin %.hex %.srec %.list
	@#echo "*** $* images generated ***"

%.bin: %.elf
	@#printf "  OBJCOPY $(*).bin\n"
	$(OBJCOPY) -Obinary $(*).elf $(*).bin && cp $(*).bin $(BINDIR)/.

%.hex: %.elf
	@#printf "  OBJCOPY $(*).hex\n"
	$(OBJCOPY) -Oihex $(*).elf $(*).hex && cp $(*).hex $(BINDIR)/.

%.srec: %.elf
	@#printf "  OBJCOPY $(*).srec\n"
	$(OBJCOPY) -Osrec $(*).elf $(*).srec && cp $(*).srec $(BINDIR)/.

%.list: %.elf
	@#printf "  OBJDUMP $(*).list\n"
	$(OBJDUMP) -S $(*).elf > $(*).list && cp $(*).list $(BINDIR)/.

%.elf: $(OBJS) $(LDSCRIPT)
	@#printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(LD) $(OBJS) $(LDFLAGS) -o $(*).elf && cp $(*).elf $(BINDIR)/.

%.o: %.c Makefile
	@#printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.cpp Makefile
	@#printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(CC) $(CFLAGS) -o $@ -c $<

SRC = $(wildcard *.c)
HDR = $(wildcard *.h)

clean:
	rm -f *.o
	rm -f *.d
	rm -f *.elf
	rm -f *.bin
	rm -f *.hex
	rm -f *.srec
	rm -f *.list
	rm -f *.orig

.PHONY: images clean