#include <kalloc.h>

#include <list.h>
#include <mem.h>
#include <page_alloc.h>

typedef struct {
    list_node_t node;

    bool allocated;
    usize size;
} block_header_t;

static list_node_t block_list = LIST_HEAD_INIT(block_list);

void *kmalloc(usize size) {
    size += sizeof(block_header_t);
    if (size > PAGE_SIZE)
        return nullptr;

    block_header_t *block;
    list_for_each_entry(&block_list, node, block) {
        if (!block->allocated && block->size >= size)
            break;
    }

    if (&block->node == &block_list) {
        block = (block_header_t *) page_alloc();
        if (!block)
            return nullptr;
        block->allocated = false;
        block->size = PAGE_SIZE;
        list_push_back(&block_list, &block->node);
    }

    block->allocated = true;

    if (block->size - size > sizeof(block_header_t)) {
        block_header_t *rem_block = (block_header_t *) ((u8 *) block + size);
        rem_block->allocated = false;
        rem_block->size = block->size - size;
        list_insert(block->node.next, &rem_block->node);
        block->size = size;
    }

    return block + 1;
}

void *kcalloc(usize size, usize n) {
    size *= n;
    void *res = kmalloc(size);
    if (!res)
        return res;
    memset(res, 0, size);
    return res;
}

void kfree(void *ptr) {
    if (!ptr)
        return;

    block_header_t *curr = (block_header_t *) ptr - 1;
    curr->allocated = false;

    if (!list_is_front(&block_list, &curr->node)) {
        block_header_t *prev =
            list_entry(curr->node.prev, block_header_t, node);
        if (!prev->allocated &&
            (block_header_t *) ((u8 *) prev + prev->size) == curr) {
            prev->size += curr->size;
            list_remove(&curr->node);
            curr = prev;
        }
    }

    if (!list_is_back(&block_list, &curr->node)) {
        block_header_t *next =
            list_entry(curr->node.next, block_header_t, node);
        if (!next->allocated &&
            (block_header_t *) ((u8 *) curr + curr->size) == next) {
            curr->size += next->size;
            list_remove(&next->node);
        }
    }
}
