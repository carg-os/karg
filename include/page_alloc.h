#pragma once

#include <types.h>

#define PAGE_SIZE 4096

i32 init_page_alloc(void);
void *page_alloc(void);
void page_free(void *ptr);
