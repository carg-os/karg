#include <drivers/tty.h>

#include <config.h>
#include <errno.h>
#include <kalloc.h>

tty_ctrl_blk_t *tty_ctrl_blks[DRIVER_DEV_CAPACITY];

static isize read(u32 num, u8 *buf, usize size) {
    if (num >= DRIVER_DEV_CAPACITY || !tty_ctrl_blks[num])
        return -ENXIO;
    tty_ctrl_blk_t *ctrl_blk = tty_ctrl_blks[num];
    for (usize i = 0; i < size; i++) {
        sem_wait(&ctrl_blk->rx_sem);
        buf[i] = ctrl_blk->rx_buf[ctrl_blk->rx_head++];
        ctrl_blk->rx_head %= TTY_BUF_SIZE;
        if (buf[i] == '\n')
            return i + 1;
    }
    return size;
}

static isize write(u32 num, const u8 *buf, usize size) {
    if (num >= DRIVER_DEV_CAPACITY || !tty_ctrl_blks[num])
        return -ENXIO;
    return ldisc_write(num, buf, size);
}

const driver_t TTY_DRIVER = {
    .read = read,
    .write = write,
    .ioctl = nullptr,
};

i32 tty_init_ctrl_blk(tty_ctrl_blk_t *ctrl_blk) {
    ctrl_blk->rx_buf = kmalloc(TTY_BUF_SIZE);
    if (!ctrl_blk->rx_buf)
        return -ENOMEM;
    ctrl_blk->rx_head = 0;
    ctrl_blk->rx_tail = 0;
    sem_init(&ctrl_blk->rx_sem);
    return 0;
}
