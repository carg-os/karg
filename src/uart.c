#include <config.h>
#include <dev.h>
#include <driver.h>
#include <platform.h>
#include <plic.h>
#include <sem.h>

#define REG(reg) *((volatile u8 *) (UART_BASE0 + UART_REG_SIZE * (reg)))
#define THR 0
#define RBR 0
#define IER 1
#define IIR 2
#define FCR 2
#define LSR 5
#define USR 31

#define IER_ERBFI 0x01

#define LSR_THRE 0x20

static sem_t rx_sem;
static u8 rx_buf[UART_RX_BUF_SIZE];
static usize rx_head = 0, rx_tail = 0;
static usize cursor_pos = 0;

static void uart_isr(u32 minor) {
    if (REG(IIR) == 0xC7) {
        REG(USR);
        return;
    }

    (void) minor;
    u8 c = REG(RBR);

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

            dev_write((dev_t){.driver = &tty_driver, .num = 0},
                      (const u8 *) "\b \b", 3);
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

        dev_write((dev_t){.driver = &tty_driver, .num = 0}, &c, 1);

        break;
    }
}

static isize uart_read(u32 minor, u8 *buf, usize size) {
    (void) minor;

    for (usize i = 0; i < size; i++) {
        sem_wait(&rx_sem);
        buf[i] = rx_buf[rx_head];
        rx_head++;
        rx_head %= UART_RX_BUF_SIZE;

        if (buf[i] == '\n')
            return i + 1;
    }

    return size;
}

static void uart_put_char(u8 c) {
    while (!(REG(LSR) & LSR_THRE))
        ;
    REG(THR) = c;
}

static isize uart_write(u32 minor, const u8 *buf, usize size) {
    (void) minor;

    for (usize i = 0; i < size; i++) {
        u8 c = buf[i];
        if (c == '\n')
            uart_put_char('\r');
        uart_put_char(c);
    }

    return size;
}

static irq_t irqs[] = {
    UART_IRQ0,
};

driver_t uart_driver = {
    .nr_devs = 1,
    .irqs = irqs,
    .isr = uart_isr,
    .read = uart_read,
    .write = uart_write,
};

i32 init_uart(void) {
    sem_init(&rx_sem);
    i32 res = driver_add(&uart_driver);
    if (res < 0)
        return res;

    plic_enable_irq(UART_IRQ0);
    REG(IER) = IER_ERBFI;
    return 0;
}
