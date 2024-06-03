#include <trap.h>

#include <csr.h>
#include <driver.h>
#include <errno.h>
#include <kprintf.h>
#include <plic.h>
#include <rv.h>
#include <syscall.h>
#include <timer.h>

typedef struct {
    u32 irq;
    void (*isr)(u32 num);
    u32 num;
} isr_entry;

static isr_entry isr_entries[256];
static u32 nr_isr_entries;

i32 init_trap(void) {
    void trap_entry(void);
    csr_write(stvec, trap_entry);
    return 0;
}

i32 trap_register_isr(u32 irq, void (*isr)(u32 num), u32 num) {
    isr_entries[nr_isr_entries++] = (isr_entry){
        .irq = irq,
        .isr = isr,
        .num = num,
    };
    return 0;
}

void trap_handler(trapframe_t *frame) {
    if (frame->scause & CSR_SCAUSE_INTR) {
        switch (frame->scause & ~CSR_SCAUSE_INTR) {
        case 5:
            timer_isr();
            break;
        case 9: {
            u32 irq = plic_claim();
            for (u32 i = 0; i < nr_isr_entries; i++) {
                if (isr_entries[i].irq != irq)
                    continue;

                isr_entries[i].isr(isr_entries[i].num);
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
