#pragma once

#include <driver.h>
#include <drivers/plic.h>
#include <types.h>
#include <utils/compiler.h>

typedef struct {
    const driver_t *driver;
    u32 num;
} dev_t;

#define make_dev(driver, num) ((dev_t){&(driver), num})

isize dev_read(dev_t dev, u8 *buf, usize size);
isize dev_write(dev_t dev, const u8 *buf, usize size);

typedef struct {
    const char *name;
    usize addr;
    irq_t irq;
    u8 reg_size;
} dev_node_t;

extern const dev_node_t DEV_TABLE[];

typedef struct {
    const char *name;
    const char *compat;
    i32 (*init)(const dev_node_t *node);
} dev_init_t;

#define dev_init(compat, func)                                                 \
    section(.init.dev)                                                         \
        used static const dev_init_t _DEV_INIT = {_MODULE_NAME, compat, func}
