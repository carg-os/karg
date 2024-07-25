#pragma once

#define PAGE_SIZE 4096

#ifndef __ASSEMBLER__
void *page_alloc(void);
void page_free(void *ptr);
#endif
