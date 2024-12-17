.syntax unified

.pool
.set BASE,              0x22028000
.set OLD_STATE,         0x2202B63C
.set STATE_PTR,         0x2202FFF8
.set usb_reset_core,    0x2000A814
.set usb_start,         0x20003D1C

.text

.global	_start
.arm
.type	_start, %function

_start:
    mov r4, pc
    sub r4, r4, #0x8

    ldr r0, =BASE
    ldr r1, L_hook
    add r1, r1, r4
    ldr r2, =hook_len
    add r2, r2, r4
    ldr r2, [r2]
    mov r7, r0
    add r6, r0, r2
    bl memcpy

    mov r0, r6
    ldr r1, L_new_name_desc
    add r1, r1, r4
    ldr r2, =new_name_desc_len
    add r2, r2, r4
    ldr r2, [r2]
    bl memcpy

    mov r0, #0x0
    mcr p15, 0, r0, c7, c5, 0

    ldr r0, =OLD_STATE
    ldr r1, =STATE_PTR
    str r0, [r1]

    str r7, [r0, #0x1c]

    ldr r0, [r0, #0x80]
    str r6, [r0, #0x8]

    ldr r0, =usb_reset_core
    blx r0

    ldr r0, =usb_start
    bx r0

L_hook:
    .long hook

L_new_name_desc:
    .long new_name_desc
