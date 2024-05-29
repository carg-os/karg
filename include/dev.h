#pragma once

#include <driver.h>
#include <types.h>

typedef struct {
    const driver_t *driver;
    u32 num;
} dev_t;

isize dev_read(dev_t dev, u8 *buf, usize size);
isize dev_write(dev_t dev, const u8 *buf, usize size);
