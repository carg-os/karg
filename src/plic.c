#include <plic.h>

#include <csr.h>
#include <platform.h>

#define REG(reg) *((volatile u32 *) PLIC_BASE + (reg))
#define INTR_PRIO(irq) irq
#define INTR_ENABLE(irq) 0x820 + irq / 32
#define PRIO_THRESHOLD 0x80400
#define INTR_CLAIM 0x80401
#define INTR_COMPLETION 0x80401

void init_plic(void) {
    csr_set_bits(sie, CSR_SIE_SEIE);
    REG(PRIO_THRESHOLD) = 0;
}

void plic_enable_irq(u32 irq) {
    REG(INTR_ENABLE(irq)) |= 1 << irq % 32;
    REG(INTR_PRIO(irq)) = 1;
}

u32 plic_claim(void) { return REG(INTR_CLAIM); }

void plic_complete(u32 irq) { REG(INTR_COMPLETION) = irq; }
