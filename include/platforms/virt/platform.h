#pragma once

#define BOOT_ENTRY_BASE 0x80200000
#define BOOT_STACK_BASE 0x88000000

#define PLIC_BASE 0x0C000000

#define UART_REG_SIZE 1
#define UART_BASE0 0x10000000
#define UART_IRQ0 10

#define TIMER_FREQ 10000000

#if !defined(__ASSEMBLER__) && !defined(__LINKER__)
#include <types.h>

i32 init_platform(void);
#endif
