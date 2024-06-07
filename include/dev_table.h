#pragma once

#include <compiler.h>
#include <types.h>

typedef struct {
    const char *name;
    usize addr;
    u32 irq;
    u8 reg_size;
} dev_node_t;

extern dev_node_t dev_table[];

typedef struct {
    const char *compat;
    i32 (*init)(const dev_node_t *node);
} dev_init_t;

#define DEV_INIT(compat, init)                                                 \
    __section(".init.dev") __used static dev_init_t _dev_init = {compat, init}
