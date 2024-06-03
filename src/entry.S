#include <boot.h>

    .section .init

    .global _start
_start:
    la t0, _bss_start
    la t1, _bss_end
1:
    sd zero, 0(t0)
    add t0, t0, 8
    bltu t0, t1, 1b

    li sp, BOOT_STACK_BASE

    tail kmain
