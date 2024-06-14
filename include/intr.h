#pragma once

#include <drivers/plic.h>
#include <types.h>

i32 intr_register_isr(irq_t irq, void (*isr)(void *data), void *data);
void intr_isr(void);
