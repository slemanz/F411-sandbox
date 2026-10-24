CC = arm-none-eabi-gcc
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy
PYTHON = python

RM = rm -rf

PROFILE  ?= debug
TARGET    = tests
DIR_BUILD = build/$(PROFILE)

ifeq ($(PROFILE), release)
    OPT = -Os -g
else
    OPT = -O0 -g3
endif

MCU      = -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
CFLAGS   = $(MCU) $(OPT) -std=gnu11 -DSTM32F411xE -Wall -Wextra -Wno-unused-parameter -Werror \
			-ffunction-sections -fdata-sections
LDFLAGS  = $(MCU) -T../drivers/Device/linker/STM32F411.ld --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections

INCLUDES =	-I../drivers/Device/cmsis/Inc \
			-I../drivers/Inc \
			-Iframework

OBJS += $(DIR_BUILD)/main_test.o
OBJS += $(DIR_BUILD)/ttest.o
OBJS += $(DIR_BUILD)/startup.o
OBJS += $(DIR_BUILD)/syscalls.o

SUITES += $(DIR_BUILD)/test_sanity.o

#DRIVERS += $(DIR_BUILD)/driver_gpio.o

PREREQ = $(DIR_BUILD)

all: $(PREREQ) $(DIR_BUILD)/$(TARGET).elf

$(PREREQ):
	mkdir -p $(PREREQ)

$(DIR_BUILD)/%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $(DIR_BUILD)/$(*).o $(*).c

$(DIR_BUILD)/%.o: framework/%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $(DIR_BUILD)/$(*).o framework/$(*).c

$(DIR_BUILD)/%.o: suites/%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $(DIR_BUILD)/$(*).o suites/$(*).c

$(DIR_BUILD)/%.o: ../drivers/Device/startup/%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $(DIR_BUILD)/$(*).o ../drivers/Device/startup/$(*).c

$(DIR_BUILD)/%.o: ../drivers/Device/system/%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $(DIR_BUILD)/$(*).o ../drivers/Device/system/$(*).c

$(DIR_BUILD)/%.o: ../drivers/Src/%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $(DIR_BUILD)/$(*).o ../drivers/Src/$(*).c

$(DIR_BUILD)/$(TARGET).elf: $(OBJS) $(SUITES) $(DRIVERS)
	$(CC) $(LDFLAGS) -o $@ $^
	$(OBJCOPY) -O binary $(DIR_BUILD)/$(TARGET).elf $(DIR_BUILD)/$(TARGET).bin
	$(SIZE) $(DIR_BUILD)/$(TARGET).elf

test: all
	$(PYTHON) run_tests.py $(DIR_BUILD)/$(TARGET).elf

test-all:
	$(MAKE) test PROFILE=debug
	$(MAKE) test PROFILE=release

clean:
	$(RM) $(DIR_BUILD)

.PHONY: all test test-all clean