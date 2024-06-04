#include <drivers/tty.h>

#include <config.h>
#include <errno.h>
#include <platform.h>

typedef struct {
    dev_t src;
    dev_t sinks[TTY_MAX_SINK + 1];
    u32 nr_sinks;
} ctrl_blk_t;

static ctrl_blk_t ctrl_blks[TTY_NR_DEVS];

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
    return 0;
}

driver_t tty_driver = {
    .nr_devs = TTY_NR_DEVS,
    .read = read,
    .write = write,
};

i32 init_tty(void) { return 0; }

i32 tty_register_src(u32 num, dev_t src) {
    if (num >= TTY_NR_DEVS)
        return -ENXIO;
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    ctrl_blk->src = src;
    return 0;
}

i32 tty_register_sink(u32 num, dev_t sink) {
    if (num >= TTY_NR_DEVS)
        return -ENXIO;
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    if (ctrl_blk->nr_sinks >= TTY_NR_DEVS)
        return -EAGAIN;
    ctrl_blk->sinks[ctrl_blk->nr_sinks++] = sink;
    return 0;
}

i32 tty_putc(u32 num, char c) {
    if (num >= TTY_NR_DEVS)
        return -ENXIO;
    isize res = write(num, (const u8 *) &c, 1);
    if (res < 0)
        return res;
    return c;
}
