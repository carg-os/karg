#include <trap.h>

#include <csr.h>
#include <errno.h>
#include <init.h>
#include <intr.h>
#include <rv.h>
#include <syscall.h>
#include <timer.h>

static i32 init(void) {
    void trap_entry(void);
    csr_write(stvec, trap_entry);
    return 0;
}

MODULE_PRE_INIT(init);

void trap_handler(trapframe_t *frame) {
    if (frame->scause & CSR_SCAUSE_INTR) {
        switch (frame->scause & ~CSR_SCAUSE_INTR) {
        case CSR_SCAUSE_TIMER_INTR:
            timer_isr();
            break;
        case CSR_SCAUSE_EXT_INTR:
            intr_isr();
            break;
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
