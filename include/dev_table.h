#pragma once

#include <compiler.h>
#include <drivers/plic.h>
#include <types.h>

typedef struct {
    const char *name;
    usize addr;
    irq_t irq;
    u8 reg_size;
} dev_node_t;

extern const dev_node_t DEV_TABLE[];

typedef struct {
    const char *compat;
    i32 (*init)(const dev_node_t *node);
} dev_init_t;

#define DEV_INIT(compat, func)                                                 \
    __section(.init.dev) __used static dev_init_t _dev_init = {compat, func}
