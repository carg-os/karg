#include <drivers/plic.h>

#include <csr.h>
#include <dev_table.h>
#include <errno.h>
#include <platform.h>

static usize base_addr = 0;

#define REG(reg) *((volatile u32 *) base_addr + (reg))
#define INTR_PRIO(irq) irq
#define INTR_ENABLE(irq) 0x820 + irq / 32
#define PRIO_THRESHOLD 0x80400
#define INTR_CLAIM 0x80401
#define INTR_COMPLETION 0x80401

i32 plic_enable_irq(u32 irq) {
    if (!base_addr)
        return -ENXIO;
    REG(INTR_ENABLE(irq)) |= 1 << irq % 32;
    REG(INTR_PRIO(irq)) = 1;
    return 0;
}

i32 plic_claim(u32 *irq) {
    if (!base_addr)
        return -ENXIO;
    *irq = REG(INTR_CLAIM);
    return 0;
}

i32 plic_complete(u32 irq) {
    if (!base_addr)
        return -ENXIO;
    REG(INTR_COMPLETION) = irq;
    return 0;
}

static i32 init_dev(const dev_node_t *node) {
    base_addr = node->addr;
    csr_set_bits(sie, CSR_SIE_SEIE);
    REG(PRIO_THRESHOLD) = 0;
    return 0;
}

DEV_INIT("plic", init_dev);
