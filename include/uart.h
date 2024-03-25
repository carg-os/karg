#ifndef UART_H_
#define UART_H_

void init_uart(void);
void uart_putc(char c);
char uart_getc(void);
void uart_handle_intr(void);

#endif // UART_H_
