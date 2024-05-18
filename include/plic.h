#pragma once

#include <types.h>

typedef u16 irq_t;

void init_plic(void);
void plic_enable_irq(irq_t irq);
irq_t plic_claim(void);
void plic_complete(irq_t irq);
