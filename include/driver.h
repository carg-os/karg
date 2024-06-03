#pragma once

#include <config.h>
#include <types.h>

typedef struct {
    u32 nr_devs;
    isize (*read)(u32 num, u8 *buf, usize size);
    isize (*write)(u32 num, const u8 *buf, usize size);
} driver_t;

extern driver_t tty_driver;
extern driver_t uart_driver;
extern driver_t fbcon_driver;
