#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(PLATFORM_QEMU)

#define BOOT_STACK_BASE 0x88000000

#define PLIC_BASE 0x0C000000
#define IRQ_UART 10

#define UART_BASE 0x10000000
#define UART_REG_SIZE 1

#define TIMER_FREQ 10000000

#elif defined(PLATFORM_MILKV_DUO)

#define BOOT_STACK_BASE 0x84000000

#define PLIC_BASE 0x70000000
#define IRQ_UART 44

#define UART_BASE 0x04140000
#define UART_REG_SIZE 4

#define TIMER_FREQ 25000000

#else

#error Unknown platform name specified

#endif

#endif // PLATFORM_H_
