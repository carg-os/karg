#pragma once

#define BOOT_STACK_BASE 0x84000000

#define PLIC_BASE 0x70000000

#define UART_REG_SIZE 4
#define UART_BASE0 0x04140000
#define UART_IRQ0 44

#define TIMER_FREQ 25000000

#if !defined(__ASSEMBLER__)
#include <types.h>

i32 init_platform(void);
#endif
