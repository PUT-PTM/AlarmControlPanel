# Project name and in the same time names of the executables.
PROJ_NAME=hal

# sources' directories
SRCSLIB = $(wildcard lib/*.c)
SRCS = $(wildcard src/*.c)
SRCS += src/startup_stm32f407xx.s # add startup file to build
SRCSCPP += $(wildcard src/*.cpp)

BUILDDIR = build
OBJSDIR = $(BUILDDIR)/objs
DEPSDIR = $(BUILDDIR)/deps
LDSRCS = ldscripts/libs.ld ldscripts/mem.ld ldscripts/sections.ld

vpath %.c src

###################################################


OBJS = $(SRCS:%.c=%.o)
OBJSCPP = $(SRCSCPP:%.cpp=%.o)
OBJSLIB = $(SRCSLIB:%.c=%.o)

# C compiler's settings
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy


# C compiler's options
CFLAGS  = -g -Wall -std=c11
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=soft 


# Cpp compiler's settings
CPP=arm-none-eabi-g++
CPPFLAGS  = -g -Wall -std=c++11
CPPFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CPPFLAGS += -mfloat-abi=soft 


# headers' directories
CINCS += -Iinclude -Iinclude/Legacy

# linker's settings
LDFLAGS = $(LDSRCS:%=-T%) -specs nosys.specs 

# advanced settings
#
# LDLIBS  = -Llib -lstm32f4
# -mfpu=fpv4-sp-d16
# CFLAGS += -ffreestanding -nostdlib

###################################################

.PHONY: all proj
.SUFFIXES:

all: proj

proj: $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(OBJS) $(OBJSLIB) $(OBJSCPP)
	@echo "  (LD) -o $@ $^"
	@$(CPP) $(CPPFLAGS) $(LDFLAGS) $(CINCS) -o $@ $^ $(LDLIBS)
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

#$(OBJS): | $(BUILDDIR)

%.o : %.c
	@echo "  (CC) $@"
	@$(CC) $(CFLAGS) $(CINCS) -c -o $@ $<

%.o : %.cpp
	@echo "  (CPP) $@"
	@$(CPP) $(CPPFLAGS) $(CINCS) -c -o $@ $<


#$(BUILDDIR):
#	mkdir $(BUILDDIR)
#	mkdir $(OBJSDIR)
#	mkdir $(DEPSDIR)

	

#-include $(SRCS:%.c=$(DEPSDIR)/%.d)

# Take care about non-existing headers.
# If a header doesn't exist, it assumes, that the header has been changed.
# see: GNU Make Manual "Rules without Commands or Prerequisites"
%.h:


clean:
	rm -f $(OBJSLIB)
	rm -f $(OBJSCPP)
	rm -rf $(BUILDDIR)
