.syntax unified

.pool
.text

.global	_start
.arm
.type	_start, %function

@ fuck gcc
_start:
    b hook
