#ifndef HOOK_H
#define HOOK_H

#include <stdint.h>

#define EFI     0

#define SRAM_BASE       0x22000000

#define GLOBAL_OBJ_PTR  0x2202FFF8
#define GLOBAL_OBJ_ADDR (*((volatile uintptr_t *)GLOBAL_OBJ_PTR))

#define TARGET_USB_SHUTDOWN 0x20008BBC
#define TARGET_STATE_SET    0x200031C8
#define TARGET_REBOOT       0x200031B8
#define TARGET_AES_DECRYPT  0x20001BC4
#define TARGET_AES_ENCRYPT  0x20001C3C
#define TARGET_JUMP         0x200031F4

#endif
