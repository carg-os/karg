#include <uart.h>

#include <config.h>
#include <dev.h>
#include <driver.h>
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

#define IER_ERBFI 0x01

#define LSR_THRE 0x20

void uart_isr(void);
i32 uart_getc(u32 minor);
i32 uart_putc(u32 minor, char c);

static sem_t rx_sem;
static char rx_buf[UART_RX_BUF_SIZE];
static usize rx_head = 0, rx_tail = 0;
static usize cursor_pos = 0;
static driver_t driver = {
    .irq = IRQ_UART,
    .isr = uart_isr,
    .getc = uart_getc,
    .putc = uart_putc,
};

i32 init_uart(void) {
    i32 res;

    sem_init(&rx_sem);
    res = driver_add(&driver);
    if (res < 0)
        return res;

    plic_enable_irq(IRQ_UART);
    REG(IER) = IER_ERBFI;
    return 0;
}

void uart_isr(void) {
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

            dev_putc(dev_init(0, 0), '\b');
            dev_putc(dev_init(0, 0), ' ');
            dev_putc(dev_init(0, 0), '\b');
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

        dev_putc(dev_init(0, 0), c);

        break;
    }
}

i32 uart_putc(u32 minor, char c) {
    (void) minor;
    i32 res;

    if (c == '\n') {
        res = dev_putc(dev_init(0, 0), '\r');
        if (res < 0)
            return res;
    }

    while (!(REG(LSR) & LSR_THRE))
        ;
    REG(THR) = c;

    return 0;
}

i32 uart_getc(u32 minor) {
    (void) minor;

    sem_wait(&rx_sem);
    char c = rx_buf[rx_head];
    rx_head++;
    rx_head %= UART_RX_BUF_SIZE;
    return c;
}
