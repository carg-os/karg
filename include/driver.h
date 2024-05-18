#pragma once

#include <plic.h>
#include <types.h>

typedef struct {
    u32 nr_devs;
    const irq_t *irqs;

    void (*isr)(u32 minor);
    i32 (*getc)(u32 minor);
    i32 (*putc)(u32 minor, char c);
} driver_t;

extern driver_t *driver_table[];
extern u32 nr_drivers;

i32 driver_add(driver_t *driver);
