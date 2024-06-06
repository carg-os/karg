#include <platform.h>

usize UART_BASES[UART_NR_DEVS] = {
    0x10000000,
};

u32 UART_IRQS[UART_NR_DEVS] = {
    10,
};
