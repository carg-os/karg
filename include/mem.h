#pragma once

#include <types.h>

static inline void *memset(void *dest, u8 val, usize size) {
    for (usize i = 0; i < size; i++) {
        ((u8 *) dest)[i] = val;
    }
    return dest;
}
