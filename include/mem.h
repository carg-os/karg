#ifndef MEM_H_
#define MEM_H_

#include <types.h>

void *memset(void *dest, u8 val, usize size) {
    for (usize i = 0; i < size; i++) {
        ((u8 *) dest)[i] = val;
    }
    return dest;
}

void *memcpy(void *dest, const void *src, usize size) {
    for (usize i = 0; i < size; i++) {
        ((u8 *) dest)[i] = ((const u8 *) src)[i];
    }
    return dest;
}

#endif // MEM_H_
