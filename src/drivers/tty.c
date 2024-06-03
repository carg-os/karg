#include <drivers/tty.h>

#include <config.h>
#include <errno.h>

typedef struct {
    dev_t src;
    dev_t sinks[TTY_MAX_SINK + 1];
    u32 nr_sinks;
} tty_dev_t;

static tty_dev_t tty_devs[TTY_NR_DEVS];

static isize tty_read(u32 num, u8 *buf, usize size) {
    tty_dev_t *dev = &tty_devs[num];
    return dev_read(dev->src, buf, size);
}

static isize tty_write(u32 num, const u8 *buf, usize size) {
    tty_dev_t *dev = &tty_devs[num];
    for (u32 i = 0; i < dev->nr_sinks; i++) {
        isize res = dev_write(dev->sinks[i], buf, size);
        if (res < 0)
            return res;
    }
    return 0;
}

void tty_register_src(u32 num, dev_t src) {
    tty_dev_t *dev = &tty_devs[num];
    dev->src = src;
}

i32 tty_register_sink(u32 num, dev_t sink) {
    tty_dev_t *dev = &tty_devs[num];
    if (dev->nr_sinks >= TTY_NR_DEVS)
        return -EAGAIN;
    dev->sinks[dev->nr_sinks++] = sink;
    return 0;
}

i32 tty_putc(u32 num, char c) {
    isize res = tty_write(num, (const u8 *) &c, 1);
    if (res < 0)
        return res;
    return c;
}

driver_t tty_driver = {
    .nr_devs = TTY_NR_DEVS,
    .read = tty_read,
    .write = tty_write,
};

i32 init_tty(void) { return 0; }
