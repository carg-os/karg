#pragma once

#define PAGE_SIZE 4096

void init_page_alloc(void);
void *page_alloc(void);
void page_free(void *ptr);
