#include <proc.h>

#include <arch/riscv/csr.h>

void proc_entry(void);

void proc_fill_stack(proc_t *proc, void *entry, usize *usp, int argc,
                     char *argv[]) {
    proc->sp -= 22;
    proc->sp[21] = (usize) proc;
    proc->sp[20] = (usize) entry;
    proc->sp[19] = (usize) usp;
    proc->sp[18] = argc;
    proc->sp[17] = (usize) argv;
    proc->sp[15] = (usize) proc_entry;
}

void proc_setup(proc_t *proc) {
    csr_set_bits(sstatus, CSR_SSTATUS_SPIE);
    if (proc->flags & PROC_FLAG_KERN) {
        csr_set_bits(sstatus, CSR_SSTATUS_SPP);
    } else {
        csr_clear_bits(sstatus, CSR_SSTATUS_SPP);
    }
}
