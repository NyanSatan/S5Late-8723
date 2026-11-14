# redefine this with your path
GNUARM_PREFIX ?= /opt/arm-gnu-toolchain-13.3.rel1-darwin-arm64-arm-none-eabi/bin/arm-none-eabi-

CONFIG_ROOT := configs
BUILD_ROOT  := build

ifneq ($(MAKECMDGOALS),clean)
    ifeq ($(CONFIG),)
        $(error No CONFIG specified)
    endif

    $(info %%%%% building for target $(CONFIG))
endif

CC := $(GNUARM_PREFIX)gcc
CFLAGS := -Wall
CFLAGS += -Wno-long-long
CFLAGS += -Wno-multichar
CFLAGS += -Os
CFLAGS += -march=armv6
CFLAGS += -fcall-used-r9
CFLAGS += -ffunction-sections
CFLAGS += -MMD

LD := $(CC)
LDFLAGS := -nostdlib
LDFLAGS += -e _start
LDFLAGS += -Tlinker.ld

OBJCOPY := $(GNUARM_PREFIX)objcopy

BUILD_PATH := $(BUILD_ROOT)/$(CONFIG)

SHELLCODE_BASE := 0x2202E9C0

SHELLCODE_OBJECTS := \
	$(BUILD_PATH)/shellcode.o \
	$(BUILD_PATH)/libc.o \
	$(BUILD_PATH)/misc.o

SHELLCODE_ELF := $(BUILD_PATH)/shellcode.elf
SHELLCODE_BINARY := $(BUILD_PATH)/shellcode.bin

HOOK_BASE := 0x22028000

HOOK_OBJECTS := \
	$(BUILD_PATH)/hook_start.o \
	$(BUILD_PATH)/hook.o \
	$(BUILD_PATH)/libc.o

HOOK_ELF := $(BUILD_PATH)/hook.elf
HOOK_BINARY := $(BUILD_PATH)/hook.bin
HOOK_HEADER := $(BUILD_PATH)/hook.h

INCLUDES := -I$(CONFIG_ROOT)/$(CONFIG) -I$(BUILD_PATH)

.PHONY: all

DIR_HELPER = mkdir -p $(@D)

all: $(SHELLCODE_BINARY)
	@echo "%%%%% done building"

$(SHELLCODE_BINARY): $(SHELLCODE_ELF)
	@echo "\textracting shellcode"
	@$(OBJCOPY) -O binary $< $@

$(SHELLCODE_ELF): $(SHELLCODE_OBJECTS)
	@echo "\tlinking shellcode"
	@$(LD) $(LDFLAGS) -o $@ -Ttext=$(SHELLCODE_BASE) $^

$(SHELLCODE_OBJECTS): | $(HOOK_HEADER)

$(HOOK_HEADER): $(HOOK_BINARY)
	@xxd -i -n _hook_bin $< $@

$(HOOK_BINARY): $(HOOK_ELF)
	@echo "\textracting hook"
	@$(OBJCOPY) -O binary $< $@

$(HOOK_ELF): $(HOOK_OBJECTS)
	@echo "\tlinking hook"
	@$(LD) $(LDFLAGS) -o $@ -Ttext=$(HOOK_BASE) $^

$(BUILD_PATH)/%.o: %.S
	@echo "\tbuilding ASM: $<"
	@$(DIR_HELPER)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_PATH)/%.o: %.c
	@echo "\tbuilding C: $<"
	@$(DIR_HELPER)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "%%%%% cleaning"
	$(shell rm -rf $(BUILD_ROOT))
	@echo "%%%%% done cleaning"

-include $(SHELLCODE_OBJECTS:.o=.d) $(HOOK_OBJECTS:.o=.d)
