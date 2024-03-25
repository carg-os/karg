#ifndef TRAP_H_
#define TRAP_H_

#include <csr.h>
#include <types.h>

typedef struct {
    usize sstatus;
    usize sepc;
    usize scause;
    usize stval;
    usize a0;
    usize a1;
    usize a2;
    usize a3;
    usize a4;
    usize a5;
    usize a6;
    usize a7;
} trapframe_t;

static inline void init_trap(void) {
    void trap_entry();
    csr_write(stvec, trap_entry);
}

#endif // TRAP_H_
