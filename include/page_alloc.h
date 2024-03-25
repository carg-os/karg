#ifndef PAGE_ALLOC_H_
#define PAGE_ALLOC_H_

#define PAGE_SIZE 4096

void init_page_alloc(void);
void *page_alloc(void);
void page_free(void *ptr);

#endif // PAGE_ALLOC_H_
