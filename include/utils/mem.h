#pragma once

#include <types.h>

void mem_set(void *dst, u8 val, usize size);
void mem_copy(void *dst, const void *src, usize size);
