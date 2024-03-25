#include <page_alloc.h>

#include <config.h>
#include <list.h>

extern u8 _heap_start;

static list_node_t free_list = LIST_HEAD_INIT(free_list);

void init_page_alloc(void) {
    list_node_t *node = (list_node_t *) &_heap_start;
    while ((u8 *) node < &_heap_start + KHEAP_SIZE) {
        list_push_back(&free_list, node);
        node = (list_node_t *) ((u8 *) node + PAGE_SIZE);
    }
}

void *page_alloc(void) {
    if (list_empty(&free_list))
        return nullptr;
    list_node_t *res = list_front(&free_list);
    list_pop_front(&free_list);
    return res;
}

void page_free(void *ptr) {
    if (!ptr)
        return;
    list_push_back(&free_list, (list_node_t *) ptr);
}
