/*
    common.h - common declarations used in the OS
*/

#ifndef COMMON_H
#define COMMON_H

#include "types.h"
#include "vga.h"

void kprintf(const char* str);

void kprintHex(uint8_t hexVal);

size_t strlen(const char* str);

#endif // COMMON_H