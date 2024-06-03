#include <config.h>
#include <dev.h>
#include <driver.h>
#include <platform.h>
#include <plic.h>
#include <sem.h>
#include <trap.h>
#include <tty.h>

#define REG(num, reg)                                                          \
    *((volatile u8 *) (UART_BASES[num] + UART_REG_SIZE * (reg)))
#define THR 0
#define RBR 0
#define IER 1
#define IIR 2
#define FCR 2
#define LSR 5

#define IER_ERBFI 0x01

#define LSR_THRE 0x20

typedef struct {
    sem_t rx_sem;
    u8 rx_buf[UART_RX_BUF_SIZE];
    usize rx_head, rx_tail;
    u32 cursor_pos;
} uart_dev_t;

static uart_dev_t uart_devs[UART_NR_DEVS];

static void uart_isr(u32 num) {
    uart_dev_t *dev = &uart_devs[num];
    u8 byte = REG(num, RBR);
    switch (byte) {
    case '\r':
        dev->rx_buf[dev->rx_tail++] = '\n';
        dev->rx_tail %= UART_RX_BUF_SIZE;
        sem_signaln(&dev->rx_sem, dev->cursor_pos + 1);
        dev->cursor_pos = 0;
        tty_putc('\n');
        break;
    case '\x7F':
        if (dev->cursor_pos > 0) {
            if (dev->rx_tail == 0) {
                dev->rx_tail = UART_RX_BUF_SIZE;
            }
            dev->rx_tail--;
            dev->cursor_pos--;
            tty_putc('\b');
            tty_putc(' ');
            tty_putc('\n');
        }
        break;
    default:
        dev->rx_buf[dev->rx_tail++] = byte;
        dev->rx_tail %= UART_RX_BUF_SIZE;
        dev->cursor_pos++;
        tty_putc(byte);
        break;
    }
}

static isize uart_read(u32 num, u8 *buf, usize size) {
    uart_dev_t *dev = &uart_devs[num];
    for (usize i = 0; i < size; i++) {
        sem_wait(&dev->rx_sem);
        buf[i] = dev->rx_buf[dev->rx_head++];
        dev->rx_head %= UART_RX_BUF_SIZE;
        if (buf[i] == '\n')
            return i + 1;
    }
    return size;
}

static void uart_put_byte(u32 num, u8 byte) {
    while (!(REG(num, LSR) & LSR_THRE))
        ;
    REG(num, THR) = byte;
}

static isize uart_write(u32 num, const u8 *buf, usize size) {
    for (usize i = 0; i < size; i++) {
        u8 byte = buf[i];
        if (byte == '\n')
            uart_put_byte(num, '\r');
        uart_put_byte(num, byte);
    }
    return size;
}

driver_t uart_driver = {
    .nr_devs = UART_NR_DEVS,
    .read = uart_read,
    .write = uart_write,
};

i32 init_uart(void) {
    for (u32 num = 0; num < UART_NR_DEVS; num++) {
        sem_init(&uart_devs[num].rx_sem);
        uart_devs[num].rx_head = 0;
        uart_devs[num].rx_tail = 0;
        uart_devs[num].cursor_pos = 0;
        plic_enable_irq(UART_IRQS[num]);
        REG(num, IER) = IER_ERBFI;
        i32 res = trap_register_isr(UART_IRQS[num], uart_isr, num);
        if (res < 0)
            return res;
    }
    return 0;
}
