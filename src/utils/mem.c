#include <utils/mem.h>

void mem_set(void *dst, u8 val, usize size) {
    for (usize i = 0; i < size; i++) {
        ((u8 *) dst)[i] = val;
    }
}

void mem_copy(void *dst, const void *src, usize size) {
    for (usize i = 0; i < size; i++) {
        ((u8 *) dst)[i] = ((const u8 *) src)[i];
    }
}
