#include <trap.h>

#include <arch/riscv/csr.h>
#include <arch/riscv/riscv.h>
#include <drivers/plic.h>
#include <errno.h>
#include <intr.h>
#include <module/init.h>
#include <module/module.h>
#include <syscall.h>
#include <timer.h>

MODULE_NAME("trap");

static i32 init(void) {
    void trampoline(void);
    csr_write(stvec, trampoline);
    return 0;
}

module_pre_init(init);

void trap_handler(trapframe_t *frame) {
    if (frame->scause & CSR_SCAUSE_INTR) {
        switch (frame->scause & ~CSR_SCAUSE_INTR) {
        case CSR_SCAUSE_TIMER_INTR:
            timer_isr();
            break;
        case CSR_SCAUSE_EXT_INTR: {
            u32 irq = plic_claim();
            intr_isr(irq);
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
