#include <uart.h>

#include <config.h>
#include <platform.h>
#include <plic.h>
#include <sem.h>

#define REG(reg) *((volatile u8 *) UART_BASE + UART_REG_SIZE * (reg))
#define THR 0
#define RBR 0
#define IER 1
#define IIR 2
#define FCR 2
#define LSR 5
#ifdef PLATFORM_MILKV_DUO
#define USR 31
#endif

#define IER_ERBFI 0x01

#define LSR_THRE 0x20

static sem_t rx_sem;
static char rx_buf[UART_RX_BUF_SIZE];
static usize rx_head = 0, rx_tail = 0;
static usize cursor_pos = 0;

void init_uart(void) {
    sem_init(&rx_sem);

    plic_enable_irq(IRQ_UART);
    REG(IER) = IER_ERBFI;
}

void uart_putc(char c) {
    if (c == '\n')
        uart_putc('\r');

    while (!(REG(LSR) & LSR_THRE))
        ;
    REG(THR) = c;
}

char uart_getc(void) {
    sem_wait(&rx_sem);
    char c = rx_buf[rx_head];
    rx_head++;
    rx_head %= UART_RX_BUF_SIZE;
    return c;
}

void uart_handle_intr(void) {
#ifdef PLATFORM_MILKV_DUO
    if (REG(IIR) == 0xC7) {
        REG(USR);
        return;
    }
#endif

    char c = REG(RBR);

    switch (c) {
    case '\x7F':
        c = '\b';
        break;
    case '\r':
        c = '\n';
        break;
    }

    switch (c) {
    case '\b':
        if (cursor_pos > 0) {
            if (rx_tail > 0) {
                rx_tail--;
            } else {
                rx_tail = UART_RX_BUF_SIZE - 1;
            }

            cursor_pos--;

            uart_putc('\b');
            uart_putc(' ');
            uart_putc('\b');
        }
        break;
    default:
        rx_buf[rx_tail] = c;
        rx_tail++;
        rx_tail %= UART_RX_BUF_SIZE;

        cursor_pos++;
        if (c == '\n') {
            sem_signaln(&rx_sem, cursor_pos);
            cursor_pos = 0;
        }

        uart_putc(c);

        break;
    }
}
