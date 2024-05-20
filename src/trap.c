#include <trap.h>

#include <csr.h>
#include <driver.h>
#include <errno.h>
#include <rv.h>
#include <syscall.h>
#include <timer.h>

i32 init_trap(void) {
    void trap_entry(void);
    csr_write(stvec, trap_entry);
    return 0;
}

void trap_handler(trapframe_t *frame) {
    if (frame->scause & CSR_SCAUSE_INTR) {
        switch (frame->scause & ~CSR_SCAUSE_INTR) {
        case 5:
            timer_isr();
            break;
        case 9: {
            irq_t irq = plic_claim();
            for (u32 i = 0; i < nr_drivers; i++) {
                if (!driver_table[i]->irqs || !driver_table[i]->isr)
                    continue;

                for (u32 j = 0; j < driver_table[i]->nr_devs; j++) {
                    if (irq != driver_table[i]->irqs[j])
                        continue;

                    driver_table[i]->isr(j);
                }
            }
            plic_complete(irq);
            break;
        }
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
