#include <intr.h>

#include <config.h>
#include <errno.h>

typedef struct {
    irq_t irq;
    void (*isr)(void *data);
    void *data;
} isr_t;

static isr_t isrs[INTR_ISR_CAPACITY];
static u32 nr_isrs = 0;

i32 intr_register_isr(irq_t irq, void (*isr)(void *data), void *data) {
    if (nr_isrs == INTR_ISR_CAPACITY)
        return -EAGAIN;
    isrs[nr_isrs].irq = irq;
    isrs[nr_isrs].isr = isr;
    isrs[nr_isrs].data = data;
    nr_isrs++;
    return 0;
}

void intr_isr(void) {
    irq_t irq = plic_claim();
    for (u32 i = 0; i < nr_isrs; i++) {
        if (isrs[i].irq == irq) {
            isrs[i].isr(isrs[i].data);
            break;
        }
    }
    plic_complete(irq);
}
