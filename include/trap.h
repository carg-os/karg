#pragma once

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

i32 init_trap(void);
i32 trap_register_isr(u32 irq, void (*isr)(u32 num), u32 num);
