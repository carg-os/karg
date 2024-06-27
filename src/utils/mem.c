#include <utils/mem.h>

void mem_set(void *dest, u8 val, usize size) {
    for (usize i = 0; i < size; i++) {
        ((u8 *) dest)[i] = val;
    }
}
