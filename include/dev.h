#pragma once

#include <types.h>

typedef u32 dev_t;

#define DEV_INIT(major, minor) (major << 20 | minor)
#define DEV_MAJOR(dev) (dev >> 20)
#define DEV_MINOR(dev) (dev & 0xFFFFF)

i32 dev_getc(dev_t dev);
i32 dev_putc(dev_t dev, char c);
