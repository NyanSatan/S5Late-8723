#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define EFI     1

#define SRAM_BASE       0x22000000

#define GLOBAL_OBJ_PTR  0x2202FFF8
#define GLOBAL_OBJ_ADDR (*((volatile uintptr_t *)GLOBAL_OBJ_PTR))

#define TARGET_USB_SHUTDOWN 0x20009F98
#define TARGET_STATE_SET    0x20003AE8
#define TARGET_REBOOT       0x20003AD8
#define TARGET_AES_DECRYPT  0x20002264
#define TARGET_AES_ENCRYPT  0x20002348
#define TARGET_JUMP         0x20003B14

#endif
