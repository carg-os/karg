#pragma once

#define BOOT_STACK_BASE 0x80000000

#define PLIC_BASE 0x10000000

#define UART_REG_SIZE 4
#define UART_BASE0 0x02500000
#define UART_IRQ0 18

#define TIMER_FREQ 24000000

#if !defined(__ASSEMBLER__)
#include <types.h>

i32 init_platform(void);
#endif
