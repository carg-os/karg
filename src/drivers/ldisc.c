#include <drivers/ldisc.h>

#include <drivers/tty.h>
#include <errno.h>
#include <mm/kalloc.h>

static i32 lazy_init_ctrl_blk(u32 num) {
    if (num >= DRIVER_DEV_CAPACITY)
        return -EAGAIN;
    if (tty_ctrl_blks[num])
        return 0;

    tty_ctrl_blk_t *tty_ctrl_blk =
        (tty_ctrl_blk_t *) kmalloc(sizeof(tty_ctrl_blk_t));
    if (!tty_ctrl_blk)
        return -ENOMEM;

    ldisc_ctrl_blk_t *ctrl_blk = &tty_ctrl_blk->ldisc_ctrl_blk;
    ctrl_blk->nr_sinks = 0;
    ctrl_blk->cursor_pos = 0;

    i32 res = tty_init_ctrl_blk(tty_ctrl_blk);
    if (res < 0)
        return res;
    tty_ctrl_blks[num] = tty_ctrl_blk;
    return 0;
}

i32 ldisc_register_src(u32 num, dev_t dev) {
    i32 res = lazy_init_ctrl_blk(num);
    if (res < 0)
        return res;
    ldisc_ctrl_blk_t *ctrl_blk = &tty_ctrl_blks[num]->ldisc_ctrl_blk;
    ctrl_blk->src = dev;
    return 0;
}

i32 ldisc_register_sink(u32 num, dev_t dev) {
    i32 res = lazy_init_ctrl_blk(num);
    if (res < 0)
        return res;
    ldisc_ctrl_blk_t *ctrl_blk = &tty_ctrl_blks[num]->ldisc_ctrl_blk;
    if (ctrl_blk->nr_sinks >= TTY_SINK_CAPACITY)
        return -EAGAIN;
    ctrl_blk->sinks[ctrl_blk->nr_sinks++] = dev;
    return 0;
}

void ldisc_recv_byte(u32 num, u8 byte) {
    tty_ctrl_blk_t *tty_ctrl_blk = tty_ctrl_blks[num];
    ldisc_ctrl_blk_t *ctrl_blk = &tty_ctrl_blk->ldisc_ctrl_blk;

    switch (byte) {
    case '\r':
        byte = '\n';
        break;
    }

    switch (byte) {
    case '\x7F':
        if (ctrl_blk->cursor_pos > 0) {
            if (tty_ctrl_blk->rx_tail == 0) {
                tty_ctrl_blk->rx_tail = TTY_BUF_SIZE;
            }
            tty_ctrl_blk->rx_tail--;
            ctrl_blk->cursor_pos--;
            ldisc_write(num, (const u8 *) "\b \b", 3);
        }
        break;
    default:
        tty_ctrl_blk->rx_buf[tty_ctrl_blk->rx_tail++] = byte;
        tty_ctrl_blk->rx_tail %= TTY_BUF_SIZE;
        ctrl_blk->cursor_pos++;
        if (byte == '\n') {
            sem_signaln(&tty_ctrl_blk->rx_sem, ctrl_blk->cursor_pos);
            ctrl_blk->cursor_pos = 0;
        }
        ldisc_write(num, &byte, 1);
        break;
    }
}

isize ldisc_write(u32 num, const u8 *buf, usize size) {
    ldisc_ctrl_blk_t *ctrl_blk = &tty_ctrl_blks[num]->ldisc_ctrl_blk;
    for (u32 i = 0; i < ctrl_blk->nr_sinks; i++) {
        isize res = dev_write(ctrl_blk->sinks[i], buf, size);
        if (res < 0)
            return res;
    }
    return size;
}
