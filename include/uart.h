#pragma once

#include <types.h>

i32 init_uart(void);
void uart_isr(void);
i32 uart_putc(u32 minor, char c);
i32 uart_getc(u32 minor);
