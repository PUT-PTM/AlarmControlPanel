# put your *.o targets here, make should handle the rest!

SRCSLIB = $(wildcard lib/*.c)
SRCS = $(wildcard src/*.c) 
SRCS += src/startup_stm32f407xx.s # add startup file to build

LDSRCS = ldscripts/libs.ld ldscripts/mem.ld ldscripts/sections.ld


# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)

PROJ_NAME=hal

# that's it, no need to change anything below this line!

###################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

LDFLAGS = $(LDSRCS:%=-T%) -specs nosys.specs 
#LDLIBS  = -Llib -lstm32f4
CFLAGS  = -g -Wall -std=c11 -DSTM32F407xx
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
#CFLAGS += -ffreestanding -nostdlib
CFLAGS += -mfloat-abi=soft 
#-mfpu=fpv4-sp-d16

###################################################


ROOT=$(shell pwd)

CFLAGS += -Iinclude -Iinclude/Legacy


OBJS = $(SRCS:%.c=%.o)
OBJSLIB = $(SRCSLIB:%.c=%.o)
###################################################

.PHONY: all proj

#all: libstm32f4.a proj
all: proj

#libstm32f4.a:
#	@$(MAKE) -C lib

proj: $(PROJ_NAME).elf

#$(PROJ_NAME).elf: $(OBJS) libstm32f4.a
$(PROJ_NAME).elf: $(OBJS) $(OBJSLIB)
	@echo "	(LD) -o $@ $^"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

%.o : %.c
	@echo "	(CC) $@"
	@$(CC) $(CFLAGS) -c -o $@  $<

clean:
	rm -f $(OBJSLIB)
	rm -f $(OBJS:%.s=)
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
