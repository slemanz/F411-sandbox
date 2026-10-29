CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy
JLINK = jlink

RM = rm -rf

PROFILE  ?= debug
TARGET    = flash
DIR_BUILD = build/$(PROFILE)

ifeq ($(PROFILE), release)
    OPT = -Os -g
else
    OPT = -O0 -g3
endif

MCU      = -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
CFLAGS   = $(MCU) $(OPT) -std=gnu11 -DSTM32F411xE -Wall -Wextra -Wno-unused-parameter -Werror \
			-ffunction-sections -fdata-sections
LDFLAGS  = $(MCU) -Tdrivers/Device/linker/STM32F411.ld --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections

INCLUDES =	-Idrivers/Device/cmsis/Inc \
			-Idrivers/Inc

OBJS += $(DIR_BUILD)/main.o
OBJS += $(DIR_BUILD)/startup.o
OBJS += $(DIR_BUILD)/syscalls.o

DRIVERS += $(DIR_BUILD)/driver_gpio.o
#DRIVERS += $(DIR_BUILD)/driver_systick.o
#DRIVERS += $(DIR_BUILD)/driver_uart.o

PREREQ = $(DIR_BUILD)

all: $(PREREQ) $(DIR_BUILD)/$(TARGET).elf

$(PREREQ):
	mkdir -p $(PREREQ)

$(DIR_BUILD)/%.o: app/Src/%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $(DIR_BUILD)/$(*).o app/Src/$(*).c

$(DIR_BUILD)/%.o: drivers/Device/startup/%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $(DIR_BUILD)/$(*).o drivers/Device/startup/$(*).c

$(DIR_BUILD)/%.o: drivers/Device/system/%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $(DIR_BUILD)/$(*).o drivers/Device/system/$(*).c

$(DIR_BUILD)/%.o: drivers/Src/%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $(DIR_BUILD)/$(*).o drivers/Src/$(*).c

$(DIR_BUILD)/$(TARGET).elf: $(OBJS) $(DRIVERS)
	$(CC) $(LDFLAGS) -o $@ $^
	$(OBJCOPY) -O binary $(DIR_BUILD)/$(TARGET).elf $(DIR_BUILD)/$(TARGET).bin
	$(SIZE) $(DIR_BUILD)/$(TARGET).elf

load:
	$(JLINK) flash.jlink

clean:
	$(RM) $(DIR_BUILD)

.PHONY: clean flash