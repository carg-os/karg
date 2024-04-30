#pragma once

#include <types.h>

void init_uart(void);
void uart_handle_intr(void);
i32 uart_putc(u32 minor, char c);
i32 uart_getc(u32 minor);
