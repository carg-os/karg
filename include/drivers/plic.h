#pragma once

#include <types.h>

void plic_enable_irq(u32 irq);
void plic_claim(u32 *irq);
void plic_complete(u32 irq);
