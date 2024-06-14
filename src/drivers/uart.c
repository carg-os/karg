#include <config.h>
#include <dev.h>
#include <driver.h>
#include <drivers/tty.h>
#include <errno.h>
#include <intr.h>
#include <module.h>
#include <sem.h>

MODULE_NAME("uart");

#define REG(num, reg)                                                          \
    *((volatile u8 *) (ctrl_blks[num].addr + ctrl_blks[num].reg_size * reg))
#define THR 0
#define RBR 0
#define IER 1
#define IIR 2
#define FCR 2
#define LSR 5

#define IER_ERBFI 0x01

#define LSR_THRE 0x20

typedef struct {
    usize addr;
    u8 reg_size;
    u8 rx_buf[UART_RX_BUF_SIZE];
    usize rx_head, rx_tail;
    sem_t rx_sem;
    u32 cursor_pos;
} ctrl_blk_t;

static ctrl_blk_t ctrl_blks[DRIVER_DEV_CAPACITY];
static u32 nr_devs = 0;

static void isr(void *data) {
    u32 num = (usize) data;
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    dev_t tty_dev = make_dev(TTY_DRIVER, num);

    u8 byte = REG(num, RBR);
    switch (byte) {
    case '\r':
        ctrl_blk->rx_buf[ctrl_blk->rx_tail++] = '\n';
        ctrl_blk->rx_tail %= UART_RX_BUF_SIZE;
        ctrl_blk->cursor_pos++;
        sem_signaln(&ctrl_blk->rx_sem, ctrl_blk->cursor_pos);
        ctrl_blk->cursor_pos = 0;
        dev_write(tty_dev, (const u8 *) "\n", 1);
        break;
    case '\x7F':
        if (ctrl_blk->cursor_pos > 0) {
            if (ctrl_blk->rx_tail == 0) {
                ctrl_blk->rx_tail = UART_RX_BUF_SIZE;
            }
            ctrl_blk->rx_tail--;
            ctrl_blk->cursor_pos--;
            dev_write(tty_dev, (const u8 *) "\b \b", 3);
        }
        break;
    default:
        ctrl_blk->rx_buf[ctrl_blk->rx_tail++] = byte;
        ctrl_blk->rx_tail %= UART_RX_BUF_SIZE;
        ctrl_blk->cursor_pos++;
        dev_write(tty_dev, &byte, 1);
        break;
    }
}

static isize read(u32 num, u8 *buf, usize size) {
    if (num >= nr_devs)
        return -ENXIO;
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
    if (num >= nr_devs)
        return -ENXIO;
    for (usize i = 0; i < size; i++) {
        u8 byte = buf[i];
        if (byte == '\n')
            put_byte(num, '\r');
        put_byte(num, byte);
    }
    return size;
}

static const driver_t DRIVER = {
    .read = read,
    .write = write,
    .ioctl = nullptr,
};

static i32 init_dev(const dev_node_t *node) {
    if (nr_devs >= DRIVER_DEV_CAPACITY)
        return -EAGAIN;
    u32 num = nr_devs++;

    ctrl_blks[num].addr = node->addr;
    ctrl_blks[num].reg_size = node->reg_size;
    ctrl_blks[num].rx_head = 0;
    ctrl_blks[num].rx_tail = 0;
    sem_init(&ctrl_blks[num].rx_sem);
    ctrl_blks[num].cursor_pos = 0;

    plic_enable_irq(node->irq);
    REG(num, IER) = IER_ERBFI;
    i32 res = intr_register_isr(node->irq, isr, (void *) (usize) num);
    if (res < 0)
        return res;

    res = tty_register_src(num, make_dev(DRIVER, num));
    if (res < 0)
        return res;
    res = tty_register_sink(num, make_dev(DRIVER, num));
    if (res < 0)
        return res;

    return 0;
}

dev_init("uart8250", init_dev);
