#pragma once

#include <driver.h>
#include <stdarg.h>
#include <types.h>

typedef struct {
    const driver_t *driver;
    u32 num;
} dev_t;

#define MAKE_DEV(driver, num) ((dev_t){&(driver), num})

isize dev_read(dev_t dev, u8 *buf, usize size);
isize dev_write(dev_t dev, const u8 *buf, usize size);
i32 dev_ioctl(dev_t dev, u32 req, ...);
i32 dev_vioctl(dev_t dev, u32 req, va_list args);
