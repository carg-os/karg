#pragma once

#include <types.h>

#define PLIC_BASE 0x0C000000

#define UART_REG_SIZE 1
#define UART_NR_DEVS 1
extern usize UART_BASES[UART_NR_DEVS];
extern u32 UART_IRQS[UART_NR_DEVS];

#define TIMER_FREQ 10000000
