#include <trap.h>

#include <driver.h>
#include <errno.h>
#include <plic.h>
#include <rv.h>
#include <syscall.h>
#include <timer.h>

void trap_handler(trapframe_t *frame) {
    if (frame->scause & CSR_SCAUSE_INTR) {
        switch (frame->scause & ~CSR_SCAUSE_INTR) {
        case 5:
            timer_isr();
            break;
        case 9:
            u32 irq = plic_claim();
            for (usize i = 0; i < nr_drivers; i++) {
                if (irq == driver_table[i]->irq && driver_table[i]->isr)
                    driver_table[i]->isr();
            }
            plic_complete(irq);
            break;
        }
    } else {
        switch (frame->scause) {
        case 8:
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
