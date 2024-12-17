.PHONY: all clean distclean

GNUARM_PREFIX ?= /opt/arm-gnu-toolchain-13.3.rel1-darwin-arm64-arm-none-eabi/bin/arm-none-eabi-

CC = $(GNUARM_PREFIX)gcc
CFLAGS = -Wall
CFLAGS += -Wno-long-long
CFLAGS += -Wno-multichar
CFLAGS += -Os
CFLAGS += -march=armv6
CFLAGS += -fcall-used-r9

LD = $(GNUARM_PREFIX)gcc
LDFLAGS = -nostdlib
PAYLOAD_LDFLAGS = -Tpayload.ld -fpie
HOOK_LDFLAGS = -Thook.ld -fpie

OBJCOPY = $(GNUARM_PREFIX)objcopy

PAYLOAD_SOURCES = \
	payload.c

PAYLOAD_OBJECTS = $(PAYLOAD_SOURCES:.c=.o)

PAYLOAD_ELF = payload.elf
PAYLOAD_BIN = payload.bin

HOOK_SOURCES = \
	hook.c

HOOK_OBJECTS = $(HOOK_SOURCES:.c=.o)

HOOK_ELF = hook.elf
HOOK_BIN = hook.bin
HOOK_HEADER = hook.h

all: $(PAYLOAD_BIN)

$(PAYLOAD_BIN): $(PAYLOAD_ELF)
	$(OBJCOPY) -O binary $< $@

$(PAYLOAD_ELF): $(PAYLOAD_OBJECTS) | start.o
	$(LD) -o $@ $(LDFLAGS) $(PAYLOAD_LDFLAGS) $^

$(PAYLOAD_OBJECTS): $(HOOK_HEADER)

$(HOOK_HEADER): $(HOOK_BIN)
	xxd -n hook -i $< $@

$(HOOK_BIN): $(HOOK_ELF)
	$(OBJCOPY) -O binary $< $@

$(HOOK_ELF): $(HOOK_OBJECTS) | hook_start.o
	$(LD) -o $@ $(LDFLAGS) $(HOOK_LDFLAGS) $^

.S.o:
	$(CC) -o $@ $(CFLAGS) -c $<

.c.o:
	$(CC) -o $@ $(CFLAGS) -c $<

clean:
	-$(RM) *.o *.elf *.a $(HOOK_HEADER)

distclean: clean
	-$(RM) *.bin
