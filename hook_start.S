.syntax unified

.pool
.text

.global	_start
.arm
.type	_start, %function

_start:
    b hook
