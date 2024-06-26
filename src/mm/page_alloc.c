#include <mm/page_alloc.h>

#include <config.h>
#include <list.h>
#include <module/init.h>
#include <module/module.h>

extern u8 _heap_start[KHEAP_SIZE];

MODULE_NAME("page_alloc");

static list_node_t free_list = list_head_init(free_list);

static i32 init(void) {
    list_node_t *node = (list_node_t *) _heap_start;
    while ((u8 *) node < _heap_start + KHEAP_SIZE) {
        list_push_back(&free_list, node);
        node = (list_node_t *) ((u8 *) node + PAGE_SIZE);
    }
    return 0;
}

module_pre_init(init);

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
