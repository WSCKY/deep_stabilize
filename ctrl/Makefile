################################################################################
# Project Makefile
# kyChu@2020-2-20
################################################################################

# Project Name
ProjName = AttCtrl

# TOP path
TopPath = $(shell pwd)
# BUILD path
BuildPath = $(TopPath)/build
# Output path
OutPath = $(TopPath)/output

#Platform
PLATFORM = -mcpu=cortex-m0 -mthumb -mfloat-abi=soft

# user definitions
-include .config

# Include Dirs
INCS = -I$(TopPath)/startup \
       -I$(TopPath)/sysconfig \
       -I$(TopPath)/driver/periph \
       -I$(TopPath)/driver/device \
       -I$(TopPath)/driver/cmsis/device \
       -I$(TopPath)/driver/cmsis/core \
       -I$(TopPath)/driver/stdriver/inc \
       -I$(TopPath)/middleware/rtu \
       -I$(TopPath)/middleware/maths \
       -I$(TopPath)/middleware/kylink \
       -I$(TopPath)/middleware/freertos/inc

ifeq ($(CONFIG_USB_IF_ENABLE),y)
INCS += -I$(TopPath)/driver/usb/app \
        -I$(TopPath)/driver/usb/drv \
        -I$(TopPath)/driver/usb/lib/core \
        -I$(TopPath)/driver/usb/lib/class
endif

ifeq ($(CONFIG_LOG_ENABLE),y)
INCS += -I$(TopPath)/middleware/log
endif

LD_INCS = -I$(TopPath)/sysconfig

# get version from git
__VERSION_STR__ := $(shell git rev-parse --abbrev-ref HEAD):$(shell git rev-parse --short HEAD)

# Global definitions
DEFS = -DSTM32 -DSTM32F0 -DSTM32F072CBTx -DSTM32F072 -DUSE_STDPERIPH_DRIVER

# External Library path
LIB_PATH := -L$(BuildPath)/rtu \
            -L$(BuildPath)/rtos \
            -L$(BuildPath)/maths \
            -L$(BuildPath)/kylink \
            -L$(BuildPath)/stdrv

ifeq ($(CONFIG_LOG_ENABLE),y)
LIB_PATH += -L$(BuildPath)/log
endif

# External Libraries
LIBS := -lrtu -lrtos -lmaths -lkylink -lstdrv

ifeq ($(CONFIG_LOG_ENABLE),y)
LIBS += -llog
endif

# Other configuration
CFGS = -mthumb -Dprintf=iprintf -fno-exceptions
LD_CFGS = --specs=nano.specs

ifeq ($(D), 1)
# Debug information
DBGS = -DDEBUG -g3
else
DBGS =
endif

# Set verbosity
ifeq ($(V), 1)
Q =
else
Q = @
# Do not print "Entering directory ...".
MAKEFLAGS += --no-print-directory
endif

# Commands
CC      := $(Q)arm-none-eabi-gcc
AS      := $(Q)arm-none-eabi-as
LD      := $(Q)arm-none-eabi-ld
AR      := $(Q)arm-none-eabi-ar
CPP     := $(Q)arm-none-eabi-cpp
SIZE    := $(Q)arm-none-eabi-size
STRIP   := $(Q)arm-none-eabi-strip -s
OBJCOPY := $(Q)arm-none-eabi-objcopy
OBJDUMP := $(Q)arm-none-eabi-objdump
OPENOCD ?= $(Q)openocd
RM      := $(Q)rm -rf
MKDIR   := $(Q)mkdir
ECHO    := $(Q)@echo
PRINTF  := $(Q)@printf
MAKE    := $(Q)make
JAVA    := $(Q)java
CAT     := $(Q)cat

# Export Commands
# -Operate
export Q
export CC
export AS
export LD
export AR
export SIZE
export OBJCOPY
export OBJDUMP
export MKDIR
export ECHO
# -Variables
export INCS
export DBGS
export CFGS
export DEFS
export PLATFORM
export TopPath

# All Target
all: $(ProjName).elf | $(BuildPath)

# All of the sources participating in the build are defined here
-include startup/subdir.mk
-include apps/subdir.mk
ifeq ($(CONFIG_USB_IF_ENABLE),y)
-include driver/usb/subdir.mk
endif
-include driver/periph/subdir.mk
-include driver/device/subdir.mk

# Add inputs and outputs from these tool invocations to the build variables 

$(BuildPath):
	$(MKDIR) -p $@

$(OutPath):
	$(MKDIR) -p $@

$(OBJ_DIRS):
	$(MKDIR) -p $@

# generate elf
$(ProjName).elf: libraries $(OBJS) | $(OutPath)
	@echo ''
	@echo ' Building $@'
	$(CPP) -P -E $(LD_INCS) $(TopPath)/LinkerScript.ld.S > $(BuildPath)/LinkerScript.lds
	$(ECHO) $(OBJS) > $(OutPath)/objects.list
	$(CC) $(PLATFORM) $(LIB_PATH) -T$(BuildPath)/LinkerScript.lds $(LD_CFGS) -Wl,-Map=$(OutPath)/output.map -Wl,--gc-sections -o $(OutPath)/$(ProjName).elf @$(OutPath)/objects.list $(LIBS) -lm
	$(MAKE) $(MAKEFLAGS) post-build

# Other Targets
clean:
	-$(RM) $(BuildPath)/ $(OutPath)/
	-@echo 'Cleaned.'

libraries:
	$(MAKE) -C middleware/rtu BUILD=$(BuildPath)/rtu
	$(MAKE) -C middleware/maths BUILD=$(BuildPath)/maths
	$(MAKE) -C middleware/kylink BUILD=$(BuildPath)/kylink
	$(MAKE) -C middleware/freertos BUILD=$(BuildPath)/rtos
	$(MAKE) -C driver/stdriver BUILD=$(BuildPath)/stdrv
ifeq ($(CONFIG_LOG_ENABLE),y)
	$(MAKE) -C middleware/log BUILD=$(BuildPath)/log
endif

BINARY := $(OutPath)/$(ProjName).bin
FIRMWARE := $(OutPath)/$(ProjName).fw

flash: all
	@st-flash --reset write $(BINARY) 0x08002000

upgrade: all
	@./../tools/flasher -f $(FIRMWARE)

linkck:
	@./../tools/linkck/Debug/linkck

monitor:
	@./../tools/monitor

post-build:
	$(ECHO) ' Generate disassembled listing for checking ...'
	$(OBJDUMP) -S $(OutPath)/$(ProjName).elf > $(OutPath)/disasm.txt
	$(ECHO) ' Generating binary and Printing size information:'
	$(OBJCOPY) -O binary $(OutPath)/$(ProjName).elf $(OutPath)/$(ProjName).bin
	$(JAVA) -cp ../tools/mkfw/ mkfw.mkfw $(OutPath)/$(ProjName).bin PLAIN
	$(SIZE) $(OutPath)/$(ProjName).elf
	-@echo ' Completed.'

.PHONY: all clean hal_lib
.SECONDARY: post-build
