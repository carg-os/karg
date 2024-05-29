#pragma once

#include <config.h>
#include <plic.h>
#include <types.h>

typedef struct {
    u32 nr_devs;
    const irq_t *irqs;

    void (*isr)(u32 num);
    isize (*read)(u32 num, u8 *buf, usize size);
    isize (*write)(u32 num, const u8 *buf, usize size);
} driver_t;

extern driver_t *driver_table[DRIVER_TABLE_SIZE];
extern u32 nr_drivers;

i32 driver_add(driver_t *driver);

extern driver_t tty_driver;
extern driver_t uart_driver;
extern driver_t fbcon_driver;
