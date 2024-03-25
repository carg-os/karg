#ifndef KALLOC_H_
#define KALLOC_H_

#include <types.h>

void *kmalloc(usize size);
void *kcalloc(usize size, usize n);
void kfree(void *ptr);

#endif // KALLOC_H_
