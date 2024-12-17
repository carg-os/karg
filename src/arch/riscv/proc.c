#include <proc.h>

#include <arch/riscv/csr.h>
#include <mm/page_alloc.h>

void proc_fill_stack(proc_t *proc, void *entry, usize usp, i32 argc,
                     char *argv) {
    void proc_entry(void);

    usize sstatus = csr_read(sstatus);
    sstatus |= CSR_SSTATUS_SPIE;
    if (proc->flags & PROC_FLAG_KERN) {
        sstatus |= CSR_SSTATUS_SPP;
    } else {
        sstatus &= ~CSR_SSTATUS_SPP;
    }

    proc->sp -= 38;
    proc->sp[31] = (usize) argv;
    proc->sp[30] = argc;
    proc->sp[27] = (usize) entry;
    proc->sp[26] = sstatus;
    proc->sp[25] = usp & ~0xF;
    proc->sp[16] = 8ULL << 60 | (usize) proc->page_table / PAGE_SIZE;
    proc->sp[15] = (usize) proc_entry;
}
