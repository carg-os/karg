#pragma once

#include <types.h>

static inline void *memset(void *dest, u8 val, usize size) {
    for (usize i = 0; i < size; i++) {
        ((u8 *) dest)[i] = val;
    }
    return dest;
}

static inline void *memcpy(void *dest, const void *src, usize size) {
    for (usize i = 0; i < size; i++) {
        ((u8 *) dest)[i] = ((const u8 *) src)[i];
    }
    return dest;
}
