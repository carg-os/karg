#include <trap.h>

#include <config.h>
#include <csr.h>
#include <drivers/plic.h>
#include <errno.h>
#include <init.h>
#include <rv.h>
#include <syscall.h>
#include <timer.h>

typedef struct {
    u32 irq;
    void (*isr)(u32 num);
    u32 num;
} isr_t;

static isr_t isrs[TRAP_ISR_CAPACITY];
static u32 nr_isrs;

static i32 init(void) {
    void trap_entry(void);
    csr_write(stvec, trap_entry);
    return 0;
}

MODULE_PRE_INIT(init);

i32 trap_register_isr(u32 irq, void (*isr)(u32 num), u32 num) {
    if (nr_isrs == TRAP_ISR_CAPACITY)
        return -EAGAIN;
    isrs[nr_isrs].irq = irq;
    isrs[nr_isrs].isr = isr;
    isrs[nr_isrs].num = num;
    nr_isrs++;
    return 0;
}

void trap_handler(trapframe_t *frame) {
    if (frame->scause & CSR_SCAUSE_INTR) {
        switch (frame->scause & ~CSR_SCAUSE_INTR) {
        case CSR_SCAUSE_TIMER_INTR:
            timer_isr();
            break;
        case CSR_SCAUSE_EXT_INTR: {
            u32 irq;
            plic_claim(&irq);
            for (u32 i = 0; i < nr_isrs; i++) {
                if (isrs[i].irq == irq) {
                    isrs[i].isr(isrs[i].num);
                    break;
                }
            }
            plic_complete(irq);
            break;
        }
        }
    } else {
        switch (frame->scause) {
        case CSR_SCAUSE_ECALL_U:
            frame->sepc += RV_INSTR_LEN;
            if (frame->a7 >= NR_SYSCALLS) {
                frame->a0 = -ENOSYS;
                break;
            }
            frame->a0 = SYSCALL_TABLE[frame->a7](frame);
            break;
        }
    }
}
