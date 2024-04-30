#pragma once

#include <types.h>

void init_plic(void);
void plic_enable_irq(u32 irq);
u32 plic_claim(void);
void plic_complete(u32 irq);
