#include <intr.h>

#include <drivers/plic.h>

void intr_enable_irq(u32 irq) { plic_enable_irq(irq); }
