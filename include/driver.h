#pragma once

#include <types.h>

typedef struct {
    isize (*read)(u32 num, u8 *buf, usize size);
    isize (*write)(u32 num, const u8 *buf, usize size);
} driver_t;

extern const driver_t TTY_DRIVER;
