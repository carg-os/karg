#ifndef PLIC_H_
#define PLIC_H_

#include <types.h>

#define IRQ_UART 44

void init_plic(void);
void plic_enable_irq(u32 irq);
u32 plic_claim(void);
void plic_complete(u32 irq);

#endif // PLIC_H_
