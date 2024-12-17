#include <stddef.h>
#include <stdbool.h>
// that's what the baddest do!
#include "libc.c"
#include "global_obj.h"

#define SRAM_BASE   0x22000000
#define MAX_OUT_LEN 0x40

enum {
    kAESKeyUser = 0,    // probably
    kAESKeyGID = 1,
    kAESKeyUID = 2
};

static void (*usb_shutdown)() = (void *)0x20009F14;
static void (*state_set)(int x, int y) = (void *)0x20003ACC;
static void (*reboot)() __attribute__((noreturn)) = (void *)0x20003ABC;

static void (*aes_encrypt)(void *in, void *out, size_t len, int key, void *user_key, void *user_iv) = (void *)0x200023E8;
static void (*aes_decrypt)(void *in, void *out, size_t len, int key, void *user_key, void *user_iv) = (void *)0x20002304;
static void (*prepare_and_jump)(uintptr_t addr) __attribute__((noreturn)) = (void *)0x20003AF8;

typedef enum {
    kHookCommandReset = 'rest',
    kHookCommandDump = 'dump',
    kHookCommandAESEncrypt = 'aese',
    kHookCommandAESDecrypt = 'aesd',
    kHookCommandCall = 'func'
} hook_cmd_t;

__attribute__((packed)) struct cmd {
    hook_cmd_t cmd;
    uint32_t args[];
};

#define EFI_FVH_OFFSET  0x28
static const char efi_fvh_magic[] = {'_', 'F', 'V', 'H'};

int hook(void **state) {
    struct cmd *cmd = *state;

    switch (cmd->cmd) {
        case kHookCommandReset: {
            usb_shutdown();
            reboot();
        }

        case kHookCommandDump: {
            void *addr = (void *)cmd->args[0];
            size_t len = cmd->args[1];

            if (len > MAX_OUT_LEN) {
                return -1;
            }

            memcpy(*state, addr, len);
            break;
        }

        case kHookCommandAESEncrypt:
        case kHookCommandAESDecrypt: {
            void *dest = *state;
            size_t len = cmd->args[0];
            int key = cmd->args[1];
            bool iv_hack = (bool)(cmd->args[2]);
            void *buf = &cmd->args[3];

            if (len > MAX_OUT_LEN) {
                return -1;
            }

            if (cmd->cmd == kHookCommandAESDecrypt) {
                aes_decrypt(buf, buf, len, key, NULL, NULL);
            } else {
                aes_encrypt(buf, buf, len, key, NULL, NULL);
            }

            int off = iv_hack ? 0x10 /* AES block size */ : 0x0;

            memcpy(dest, buf + off, len - off);
            break;
        }

        case kHookCommandCall: {
            void *func = (void *)cmd->args[0];
            uint32_t *args = &cmd->args[1];

            uint32_t (*tramp)(...) = func;

            uint32_t ret = tramp(
                args[0],
                args[1],
                args[2],
                args[3],
                args[4],
                args[5],
                args[6],
                args[7]
            );

            memcpy(*state, &ret, sizeof(ret));
            break;
        }

        default: {
            if (*(uint32_t *)(SRAM_BASE + EFI_FVH_OFFSET) == *(uint32_t *)efi_fvh_magic) {
                usb_shutdown();
                state_set(1, 0);
                prepare_and_jump(SRAM_BASE);
            }

            return -1;
        }
    }

    return 0;
}
