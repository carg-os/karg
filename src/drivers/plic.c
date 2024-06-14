#include <drivers/plic.h>

#include <csr.h>
#include <dev.h>
#include <module.h>

MODULE_NAME("plic");

static usize base_addr = 0;

#define REG(reg) *((volatile u32 *) (base_addr + reg))
#define INTR_PRIO(irq) (irq * 4)
#define INTR_ENABLE(irq) (0x2080 + irq / 32 * 4)
#define PRIO_THRESHOLD 0x201000
#define INTR_CLAIM 0x201004
#define INTR_COMPLETION 0x201004

void plic_enable_irq(irq_t irq) {
    REG(INTR_ENABLE(irq)) |= 1 << irq % 32;
    REG(INTR_PRIO(irq)) = 1;
}

irq_t plic_claim() { return REG(INTR_CLAIM); }

void plic_complete(irq_t irq) { REG(INTR_COMPLETION) = irq; }

static i32 init_dev(const dev_node_t *node) {
    base_addr = node->addr;
    csr_set_bits(sie, CSR_SIE_SEIE);
    REG(PRIO_THRESHOLD) = 0;
    return 0;
}

dev_init("plic", init_dev);
