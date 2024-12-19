#ifndef HOOK_H
#define HOOK_H

#include <stdint.h>

#define EFI     1

#define SRAM_BASE       0x22000000

#define GLOBAL_OBJ_PTR  0x2202FFF8
#define GLOBAL_OBJ_ADDR (*((volatile uintptr_t *)GLOBAL_OBJ_PTR))

#define TARGET_USB_SHUTDOWN 0x20009F14
#define TARGET_STATE_SET    0x20003ACC
#define TARGET_REBOOT       0x20003ABC
#define TARGET_AES_DECRYPT  0x20002304
#define TARGET_AES_ENCRYPT  0x200023E8
#define TARGET_JUMP         0x20003AF8

#endif
