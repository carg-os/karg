#pragma once

#include <types.h>

void intr_enable_irq(u32 irq);
i32 intr_register_isr(u32 irq, void (*isr)(void *data), void *data);
void intr_isr(u32 irq);
