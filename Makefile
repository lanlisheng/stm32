CROSS_COMPILE ?= arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
GDB = $(CROSS_COMPILE)gdb
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size

MCU = cortex-m3
OUTDIR = obj
WRAPPER = .
#USE_DSPLIB = YES
USE_MATHLIB = NO
DEBUG = YES

CFLAGS = 
SRC = 
MODEL = MD
OPENOCD = openocd
OPENOCD_FLAGS = 
# Basename for the resulting .elf/.bin/.hex file
RESULT ?= stm32f103

USBSERIAL = /dev/tty.usbserial-0001

ifeq ($(MCU), cortex-m3)
STARTUP = $(WRAPPER)/startup/stm32f10x
USER = $(WRAPPER)/user
APP = $(WRAPPER)/app
OS = $(WRAPPER)/os
HAL_DRIVER = $(WRAPPER)/driver
STD_DRIVER = $(WRAPPER)/Libraries_stm32f10x/STM32F10x_StdPeriph_Driver
CMSIS = $(WRAPPER)/Libraries_stm32f10x/CMSIS
# OPENOCD_FLAGS = -f ../stlink-v2.cfg -f ../stm32f1x_stlink.cfg
endif

# ifeq ($(USE_DSPLIB), YES)
# CFLAGS += -DUSE_DSPLIB
# include ../DSP_Lib/library.mk
# endif

ifeq ($(MCU), cortex-m3)
LINKER_SCRIPT = stm32f10x.ld
SRC += $(STARTUP)/startup_stm32f10x_$(MODEL).s
SRC += $(CMSIS)/CM3/CoreSupport/core_cm3.c
SRC += $(STARTUP)/system_stm32f10x.c
SRC += $(STARTUP)/stm32f10x_it.c
endif

# SRC += $(wildcard $(STARTUP)/*.c)

# SRC += $(wildcard $(STD_DRIVER)/src/*.c)
SRC += $(STD_DRIVER)/src/misc.c
SRC += $(STD_DRIVER)/src/stm32f10x_adc.c
SRC += $(STD_DRIVER)/src/stm32f10x_dma.c
SRC += $(STD_DRIVER)/src/stm32f10x_exti.c
SRC += $(STD_DRIVER)/src/stm32f10x_flash.c
SRC += $(STD_DRIVER)/src/stm32f10x_gpio.c
SRC += $(STD_DRIVER)/src/stm32f10x_tim.c
SRC += $(STD_DRIVER)/src/stm32f10x_usart.c
SRC += $(STD_DRIVER)/src/stm32f10x_rcc.c
SRC += $(STD_DRIVER)/src/stm32f10x_spi.c

SRC += $(wildcard $(USER)/*.c)
SRC += $(wildcard $(OS)/*.c)
SRC += $(wildcard $(APP)/*.c)
SRC += $(wildcard $(HAL_DRIVER)/*.c)
SRC += $(wildcard ./*.c)

EXTRAINCDIRS += $(STARTUP)/inc/
EXTRAINCDIRS += $(STD_DRIVER)/inc/
EXTRAINCDIRS += $(USER)/
EXTRAINCDIRS += $(OS)/
EXTRAINCDIRS += $(APP)/
EXTRAINCDIRS += $(WRAPPER)/driver/inc/
# EXTRAINCDIRS += $(WRAPPER)/user/

ifeq ($(MCU), cortex-m3)
# EXTRAINCDIRS += $(CMSIS)/CM3/DeviceSupport/ST/STM32F10x
EXTRAINCDIRS += $(CMSIS)/CM3/CoreSupport/
endif

CFLAGS += -O
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS)) -I.

CFLAGS += -mapcs-frame
CFLAGS += -fomit-frame-pointer
CFLAGS += -ffast-math

CFLAGS += -Wall
#CFLAGS += -Werror
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wdouble-promotion
CFLAGS += -std=gnu99
CFLAGS += -MD -MP -MF $(OUTDIR)/dep/$(@F).d
CFLAGS += -mthumb

#ifeq ($(MCU), cortex-m4)
#CFLAGS += -nostartfiles
#CFLAGS += -DSTM32F40_41xxx
#CFLAGS += -DUSE_STDPERIPH_DRIVER
#CFLAGS += -mcpu=$(MCU) -mfpu=fpv4-sp-d16 -mfloat-abi=hard -march=armv7e-m
#CFLAGS += -D__FPU_PRESENT=1
#CFLAGS += -DARM_MATH_CM4
#CFLAGS += -DARM_MATH_MATRIX_CHECK
#CFLAGS += -DARM_MATH_ROUNDING
#CFLAGS += -DUNALIGNED_SUPPORT_DISABLE
#CFLAGS += -DBLOCK_SIZE=512
#endif

ifeq ($(MCU), cortex-m3)
CFLAGS += -DSTM32F10X_$(MODEL)
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -mcpu=$(MCU) 
endif

CFLAGS += -DSMART_SYSTEM_ID=1
ifeq ($(DEBUG), YES)
CFLAGS += -g
endif

# Linker flags.
LDFLAGS += -lc -lgcc -lm
LDFLAGS += -Wl,--warn-common
LDFLAGS += -Wl,--fatal-warnings

##################################################################

# List of all source files.
ALLSRC     = $(SRC)
# List of all source files without directory and file-extension.
# notdir
ALLSRCBASE = $(notdir $(basename $(ALLSRC)))
# Define all object files.
# addprefix
# All object files specified above are prefixed the intermediate directory
ALLOBJ     = $(addprefix $(OUTDIR)/, $(addsuffix .o, $(ALLSRCBASE)))

##################################################################~


# So that the "build depends on the makefile" trick works no matter the name of
# the makefile
THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))

all: build

build: $(RESULT).elf $(RESULT).bin $(RESULT).hex $(RESULT).lst

$(RESULT).elf: $(ALLOBJ) $(LINKER_SCRIPT) $(THIS_MAKEFILE)
	@$(CC) -Wl,-T$(LINKER_SCRIPT) $(CFLAGS) $(ALLOBJ) -o $@ $(LDFLAGS)

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.lst: %.elf
	$(OBJDUMP) -x -S $(RESULT).elf > $@

size: $(RESULT).elf
	$(SIZE) $(RESULT).elf

clean:
	del  $(RESULT).elf
	del  $(RESULT).bin
	del  $(RESULT).hex
	del  $(RESULT).lst
	del   obj\*.o
erase:
	st-flash erase

ifeq ($(MCU), cortex-m3)
flash: all
	st-flash write $(RESULT).bin 0x800C000
endif

ifeq ($(MCU), cortex-m4)
flash: all
	st-flash write $(RESULT).bin 0x8020000
endif

flash_com: all
	~/Projects/c/stm32_flash/stm32_flash $(USBSERIAL):115200 $(RESULT).bin

	
ifneq ($(OPENOCD),)
debug: $(RESULT).elf
	$(OPENOCD) $(OPENOCD_FLAGS) & $(GDB) $(RESULT).elf -ex "target remote localhost:3333" -ex "load"
endif

##################################################################

define COMPILE_C_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo "CC $$<"
	@$(CC) -c $(CFLAGS) $$< -lm -lc -o $$@ 
endef

# Compile: create object files from C source files.
$(foreach src, $(SRC), $(eval $(call COMPILE_C_TEMPLATE, $(src))))

##################################################################



.PHONY: all build size clean install erase flash




