#include <drivers/uart.h>

#include <config.h>
#include <driver.h>
#include <drivers/tty.h>
#include <platform.h>
#include <plic.h>
#include <sem.h>
#include <trap.h>

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
} ctrl_blk_t;

static ctrl_blk_t ctrl_blks[UART_NR_DEVS];

static void isr(u32 num) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    u8 byte = REG(num, RBR);
    switch (byte) {
    case '\r':
        ctrl_blk->rx_buf[ctrl_blk->rx_tail++] = '\n';
        ctrl_blk->rx_tail %= UART_RX_BUF_SIZE;
        ctrl_blk->cursor_pos++;
        sem_signaln(&ctrl_blk->rx_sem, ctrl_blk->cursor_pos);
        ctrl_blk->cursor_pos = 0;
        tty_putc(num, '\n');
        break;
    case '\x7F':
        if (ctrl_blk->cursor_pos > 0) {
            if (ctrl_blk->rx_tail == 0) {
                ctrl_blk->rx_tail = UART_RX_BUF_SIZE;
            }
            ctrl_blk->rx_tail--;
            ctrl_blk->cursor_pos--;
            tty_putc(num, '\b');
            tty_putc(num, ' ');
            tty_putc(num, '\n');
        }
        break;
    default:
        ctrl_blk->rx_buf[ctrl_blk->rx_tail++] = byte;
        ctrl_blk->rx_tail %= UART_RX_BUF_SIZE;
        ctrl_blk->cursor_pos++;
        tty_putc(num, byte);
        break;
    }
}

static isize read(u32 num, u8 *buf, usize size) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    for (usize i = 0; i < size; i++) {
        sem_wait(&ctrl_blk->rx_sem);
        buf[i] = ctrl_blk->rx_buf[ctrl_blk->rx_head++];
        ctrl_blk->rx_head %= UART_RX_BUF_SIZE;
        if (buf[i] == '\n')
            return i + 1;
    }
    return size;
}

static void put_byte(u32 num, u8 byte) {
    while (!(REG(num, LSR) & LSR_THRE))
        ;
    REG(num, THR) = byte;
}

static isize write(u32 num, const u8 *buf, usize size) {
    for (usize i = 0; i < size; i++) {
        u8 byte = buf[i];
        if (byte == '\n')
            put_byte(num, '\r');
        put_byte(num, byte);
    }
    return size;
}

static driver_t driver = {
    .nr_devs = UART_NR_DEVS,
    .read = read,
    .write = write,
    .ioctl = nullptr,
};

i32 init_uart(void) {
    for (u32 num = 0; num < UART_NR_DEVS; num++) {
        sem_init(&ctrl_blks[num].rx_sem);
        ctrl_blks[num].rx_head = 0;
        ctrl_blks[num].rx_tail = 0;
        ctrl_blks[num].cursor_pos = 0;
        plic_enable_irq(UART_IRQS[num]);
        REG(num, IER) = IER_ERBFI;
        i32 res = trap_register_isr(UART_IRQS[num], isr, num);
        if (res < 0)
            return res;
        dev_t dev = {.driver = &driver, .num = num};
        tty_register_src(num, dev);
        res = tty_register_sink(num, dev);
        if (res < 0)
            return res;
    }
    return 0;
}
