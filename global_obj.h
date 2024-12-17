#ifndef GLOBAL_OBJ_H
#define GLOBAL_OBJ_H

#include <stdint.h>

#define GLOBAL_OBJ_PTR  0x2202FFF8

#define GLOBAL_OBJ_ADDR     (*((volatile uintptr_t *)GLOBAL_OBJ_PTR))

#endif
