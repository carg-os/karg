#pragma once

#include <stddef.h>
#include <types.h>

typedef struct list_node {
    struct list_node *prev, *next;
} list_node_t;

#define list_head_init(name) {&name, &name}

static inline void list_init_head(list_node_t *head) {
    head->prev = head;
    head->next = head;
}

static inline list_node_t *list_front(list_node_t *head) { return head->next; }

static inline list_node_t *list_back(list_node_t *head) { return head->prev; }

static inline bool list_empty(list_node_t *head) {
    return head == list_front(head);
}

static inline bool list_is_front(list_node_t *head, list_node_t *node) {
    return head->next == node;
}

static inline bool list_is_back(list_node_t *head, list_node_t *node) {
    return head->prev == node;
}

#define list_entry(node, type, member)                                         \
    ((type *) ((u8 *) (node) - offsetof(type, member)))
#define list_first_entry(head, type, member)                                   \
    list_entry(list_front(head), type, member)
#define list_last_entry(head, type, member)                                    \
    list_entry(list_back(head), type, member)

static inline void list_insert(list_node_t *pos, list_node_t *node) {
    pos->prev->next = node;
    node->prev = pos->prev;
    node->next = pos;
    pos->prev = node;
}

static inline void list_push_front(list_node_t *head, list_node_t *node) {
    list_insert(list_front(head), node);
}

static inline void list_push_back(list_node_t *head, list_node_t *node) {
    list_insert(head, node);
}

static inline void list_remove(list_node_t *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static inline void list_remove_init(list_node_t *node) {
    list_remove(node);
    node->prev = node;
    node->next = node;
}

static inline void list_pop_front(list_node_t *head) {
    list_remove(list_front(head));
}

static inline void list_pop_back(list_node_t *head) {
    list_remove(list_back(head));
}

#define list_for_each(head, node)                                              \
    for (node = (head)->next; node != (head); node = node->next)
#define list_for_each_entry(head, member, entry)                               \
    for (entry = list_first_entry(head, typeof(*entry), member);               \
         &entry->member != (head);                                             \
         entry = list_entry(entry->member.next, typeof(*entry), member))
