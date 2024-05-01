#pragma once

#include <types.h>

typedef struct {
    u32 irq;
    void (*isr)(void);
    i32 (*getc)(u32 minor);
    i32 (*putc)(u32 minor, char c);
} driver_t;

extern driver_t *driver_table[];
extern usize nr_drivers;

i32 driver_add(driver_t *driver);
