#!/usr/bin/env python3

# my satanic build system,
# sorry in advance!

PLATFORMS = ["s5l8723", "s5l8443"]

SHELLCODE_BASE = 0x2202E9C0
HOOK_BASE = 0x22028000

# Overridable via "GNUARM_PREFIX" env var
GNUARM_PREFIX_DEFAULT = "/opt/arm-gnu-toolchain-13.3.rel1-darwin-arm64-arm-none-eabi/bin/arm-none-eabi-"

CC = "gcc"
LD = CC
OBJCOPY = "objcopy"

# I am praying this set of option is good enough
# to make damn gcc linker always output _start
# in the very beginning

CFLAGS = [
    "-Wall",
    "-Wno-long-long",
    "-Wno-multichar",
    "-Os",
    "-march=armv6",
    "-fcall-used-r9",
    "-ffunction-sections"
]

LDFLAGS = [
    "-nostdlib",
    "-e",
    "_start",
    "-Tlinker.ld"
]

CONFIG_ROOT = "configs"
BUILD_ROOT = "build"

def build():
    gnuarm_path = os.getenv("GNUARM_PREFIX", GNUARM_PREFIX_DEFAULT)
    cc = gnuarm_path + CC
    ld = gnuarm_path + LD
    objcopy = gnuarm_path + OBJCOPY

    for platform in PLATFORMS:
        config_root = Path(CONFIG_ROOT) / platform
        build_root = Path(BUILD_ROOT) / platform
        build_root.mkdir(parents=True, exist_ok=True)

        def build_hook():
            run(cc, "-c", "hook_start.S", "-o", build_root / "hook_start.o", *CFLAGS)
            run(cc, "-c", "hook.c", "-include", config_root / "hook.h", "-o", build_root / "hook.o", *CFLAGS)
            run(ld, "-o", build_root / "hook.elf", "-Ttext=0x%x" % HOOK_BASE, build_root / "hook_start.o", build_root / "hook.o", *LDFLAGS)
            run(objcopy, "-O", "binary", build_root / "hook.elf", build_root / "hook.bin")

        def build_shellcode():
            # otherwise this bastard puts the entire input path into symbols' name,
            # and there doesn't seem to be a way to override it!
            with cwd(build_root):
                run(objcopy, "-I", "binary", "-O", "elf32-littlearm", "hook.bin", "hook_bin.o")
            
            run(cc, "-c", "shellcode.S", "-include", config_root / "config.S", "-o", build_root / "shellcode.o", *CFLAGS)
            run(cc, "-c", "libc.c", "-include", config_root / "config.h", "-o", build_root / "globals.o", *CFLAGS)
            run(ld, "-o", build_root / "shellcode.elf", "-Ttext=0x%x" % SHELLCODE_BASE, build_root / "shellcode.o", build_root / "globals.o", build_root / "hook_bin.o", *LDFLAGS)
            run(objcopy, "-O", "binary", build_root / "shellcode.elf", build_root / "shellcode.bin")

        build_hook()
        build_shellcode()

def run(program: str, *args: list[str]):
    subprocess.run([program] + [*args], check=True)

import os
import contextlib
import subprocess
from pathlib import Path

@contextlib.contextmanager
def cwd(dir: str):
    old_dir = os.getcwd()

    os.chdir(dir)

    try:
        yield
    finally:
        os.chdir(old_dir)

if __name__ == "__main__":
    build()
