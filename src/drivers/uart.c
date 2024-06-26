#include <config.h>
#include <drivers/ldisc.h>
#include <errno.h>
#include <intr.h>
#include <module/module.h>

MODULE_NAME("uart");

#define reg(num, reg)                                                          \
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
} ctrl_blk_t;

static ctrl_blk_t ctrl_blks[DRIVER_DEV_CAPACITY];
static u32 nr_devs = 0;

static void isr(void *data) {
    u32 num = (usize) data;
    ldisc_recv_byte(num, reg(num, RBR));
}

static isize write(u32 num, const u8 *buf, usize size) {
    for (usize i = 0; i < size; i++) {
        while (!(reg(num, LSR) & LSR_THRE))
            ;
        reg(num, THR) = buf[i];
    }
    return size;
}

static const driver_t DRIVER = {
    .read = nullptr,
    .write = write,
};

static i32 init_dev(const dev_node_t *node) {
    if (nr_devs >= DRIVER_DEV_CAPACITY)
        return -EAGAIN;
    u32 num = nr_devs++;

    ctrl_blks[num].addr = node->addr;
    ctrl_blks[num].reg_size = node->reg_size;

    reg(num, IER) = IER_ERBFI;
    plic_enable_irq(node->irq);
    i32 res = intr_register_isr(node->irq, isr, (void *) (usize) num);
    if (res < 0)
        return res;

    ldisc_config_t config = {
        .crlf = true,
    };
    res = ldisc_register_src(num, (ldisc_dev_t){make_dev(DRIVER, num), config});
    if (res < 0)
        return res;
    res =
        ldisc_register_sink(num, (ldisc_dev_t){make_dev(DRIVER, num), config});
    if (res < 0)
        return res;

    return 0;
}

dev_init("uart8250", init_dev);
