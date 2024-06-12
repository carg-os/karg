#pragma once

#include <stdarg.h>
#include <types.h>

typedef struct {
    isize (*read)(u32 num, u8 *buf, usize size);
    isize (*write)(u32 num, const u8 *buf, usize size);
    i32 (*ioctl)(u32 num, u32 req, va_list args);
} driver_t;

extern driver_t tty_driver;
