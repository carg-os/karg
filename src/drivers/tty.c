#include <drivers/tty.h>

#include <config.h>
#include <errno.h>
#include <kalloc.h>

typedef struct {
    dev_t src;
    dev_t sinks[TTY_SINK_CAPACITY];
    u32 nr_sinks;
} ctrl_blk_t;

static ctrl_blk_t *ctrl_blks[DRIVER_DEV_CAPACITY];

static isize read(u32 num, u8 *buf, usize size) {
    if (num >= DRIVER_DEV_CAPACITY || !ctrl_blks[num])
        return -ENXIO;
    ctrl_blk_t *ctrl_blk = ctrl_blks[num];
    return dev_read(ctrl_blk->src, buf, size);
}

static isize write(u32 num, const u8 *buf, usize size) {
    if (num >= DRIVER_DEV_CAPACITY || !ctrl_blks[num])
        return -ENXIO;
    ctrl_blk_t *ctrl_blk = ctrl_blks[num];
    for (u32 i = 0; i < ctrl_blk->nr_sinks; i++) {
        isize res = dev_write(ctrl_blk->sinks[i], buf, size);
        if (res < 0)
            return res;
    }
    return size;
}

const driver_t TTY_DRIVER = {
    .read = read,
    .write = write,
    .ioctl = nullptr,
};

static i32 lazy_init_ctrl_blk(u32 num) {
    if (num >= DRIVER_DEV_CAPACITY)
        return -EAGAIN;
    if (ctrl_blks[num])
        return 0;
    ctrl_blk_t *ctrl_blk = (ctrl_blk_t *) kmalloc(sizeof(ctrl_blk_t));
    if (!ctrl_blk)
        return -ENOMEM;
    ctrl_blk->nr_sinks = 0;
    ctrl_blks[num] = ctrl_blk;
    return 0;
}

i32 tty_register_src(u32 num, dev_t src) {
    i32 res = lazy_init_ctrl_blk(num);
    if (res < 0)
        return res;
    ctrl_blk_t *ctrl_blk = ctrl_blks[num];
    ctrl_blk->src = src;
    return 0;
}

i32 tty_register_sink(u32 num, dev_t sink) {
    i32 res = lazy_init_ctrl_blk(num);
    if (res < 0)
        return res;
    ctrl_blk_t *ctrl_blk = ctrl_blks[num];
    if (ctrl_blk->nr_sinks >= TTY_SINK_CAPACITY)
        return -EAGAIN;
    ctrl_blk->sinks[ctrl_blk->nr_sinks++] = sink;
    return 0;
}
