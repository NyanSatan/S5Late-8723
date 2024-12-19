/*
 * This is what getting called on DFU_UPLOAD request,
 * this way we can return 0x40 bytes of data back to
 * the host and do some custom action before
 */

#include <stddef.h>
#include <stdbool.h>
// that's what the baddest do!
#include "libc.c"

// XXX length above this didn't work on S5L8442
#define MAX_OUT_LEN 0x40

enum {
    kAESKeyUser = 0,    // probably
    kAESKeyGID = 1,
    kAESKeyUID = 2
};

static void (*usb_shutdown)() = (void *)TARGET_USB_SHUTDOWN;

static void (*state_set)(int x, int y) = (void *)TARGET_STATE_SET;
static void (*prepare_and_jump)(uintptr_t addr) __attribute__((noreturn)) = (void *)TARGET_JUMP;
static void (*reboot)() __attribute__((noreturn)) = (void *)TARGET_REBOOT;

static void (*aes_decrypt)(void *in, void *out, size_t len, int key, void *user_key, void *user_iv) = (void *)TARGET_AES_DECRYPT;
static void (*aes_encrypt)(void *in, void *out, size_t len, int key, void *user_key, void *user_iv) = (void *)TARGET_AES_ENCRYPT;

/* commands we accept, add more as needed */
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

/* iPod shuffles do not use EFI */
#if EFI
#define EFI_FVH_OFFSET  0x28
static const char efi_fvh_magic[] = {'_', 'F', 'V', 'H'};
#else
#define RESET_VECTOR    0xEA00000B
#endif

int hook(void **state) {
    /* The first pointer in the DFU state points to download buffer */
    struct cmd *cmd = *state;

    switch (cmd->cmd) {
        /* This seems to always reboot back into DFU even on nanos */
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

        /*
         * S5L8723 (maybe others too) seems to have problems with
         * user-supplied IVs, so we have to use an ugly workaround
         * which lowers our poor bandwith even further
         *
         * I stole this hack from wInd3x
         */
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

            /* let's try to at least avoid transmitting garbage back to host */
            int off = iv_hack ? 0x10 /* AES block size */ : 0x0;

            // XXX can this be avoided now that aes_* functions have an output arg?
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

        /*
         * trying to see if host supplied us with something looking like 2nd-stage bootloader
         *
         * XXX redesign this mechanism to allow fully custom images to be loaded
         * XXX what if image is not yet completely copied into load area?!
         */
        default: {
#if EFI
            if (*(uint32_t *)(SRAM_BASE + EFI_FVH_OFFSET) == *(uint32_t *)efi_fvh_magic) {
#else
            if (*(uint32_t *)(SRAM_BASE) == RESET_VECTOR) {
#endif
                usb_shutdown();
                state_set(1, 0);
                prepare_and_jump(SRAM_BASE);
            }

            return -1;
        }
    }

    return 0;
}
