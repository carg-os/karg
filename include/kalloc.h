#pragma once

#include <types.h>

void *kmalloc(usize size);
void *kcalloc(usize size, usize n);
void kfree(void *ptr);
