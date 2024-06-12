#include <drivers/tty.h>

#include <config.h>
#include <errno.h>
#include <init.h>

typedef struct {
    dev_t src;
    dev_t sinks[TTY_SINK_CAPACITY];
    u32 nr_sinks;
} ctrl_blk_t;

static ctrl_blk_t ctrl_blks[DRIVER_DEV_CAPACITY];
static u32 nr_devs = 0;

static isize read(u32 num, u8 *buf, usize size) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    return dev_read(ctrl_blk->src, buf, size);
}

static isize write(u32 num, const u8 *buf, usize size) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    for (u32 i = 0; i < ctrl_blk->nr_sinks; i++) {
        isize res = dev_write(ctrl_blk->sinks[i], buf, size);
        if (res < 0)
            return res;
    }
    return size;
}

driver_t tty_driver = {
    .read = read,
    .write = write,
    .ioctl = nullptr,
};

static i32 init(void) { return 0; }

module_init(init);

static void lazy_init_ctrl_blks(u32 num) {
    if (num < nr_devs)
        return;

    for (u32 i = nr_devs; i <= num; i++) {
        ctrl_blk_t *ctrl_blk = &ctrl_blks[i];
        ctrl_blk->src.driver = nullptr;
        ctrl_blk->nr_sinks = 0;
    }
    nr_devs = num + 1;
}

i32 tty_register_src(u32 num, dev_t src) {
    if (num >= DRIVER_DEV_CAPACITY)
        return -EAGAIN;
    lazy_init_ctrl_blks(num);
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    ctrl_blk->src = src;
    return 0;
}

i32 tty_register_sink(u32 num, dev_t sink) {
    if (num >= DRIVER_DEV_CAPACITY)
        return -EAGAIN;
    lazy_init_ctrl_blks(num);
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    if (ctrl_blk->nr_sinks >= TTY_SINK_CAPACITY)
        return -EAGAIN;
    ctrl_blk->sinks[ctrl_blk->nr_sinks++] = sink;
    return 0;
}
