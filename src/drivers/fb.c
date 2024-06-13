#include <drivers/fb.h>

#include <config.h>
#include <errno.h>
#include <init.h>

typedef struct {
    dev_t dev;
} ctrl_blk_t;

static ctrl_blk_t ctrl_blks[DRIVER_DEV_CAPACITY];
static u32 nr_devs = 0;

static void (*probes[FB_DEV_PROBE_CAPACITY])(dev_t dev);
static u32 nr_dev_probes;

static i32 ioctl(u32 num, u32 req, va_list args) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    return dev_vioctl(ctrl_blk->dev, req, args);
}

static driver_t driver = {
    .read = nullptr,
    .write = nullptr,
    .ioctl = ioctl,
};

i32 fb_register_dev(dev_t dev) {
    if (nr_devs == DRIVER_DEV_CAPACITY)
        return -EAGAIN;
    u32 num = nr_devs++;
    ctrl_blks[num].dev = dev;
    for (u32 i = 0; i < nr_dev_probes; i++) {
        probes[i](make_dev(driver, num));
    }
    return 0;
}

i32 fb_register_dev_probe(void (*probe)(dev_t dev)) {
    if (nr_dev_probes == FB_DEV_PROBE_CAPACITY)
        return -EAGAIN;
    probes[nr_dev_probes++] = probe;
    for (u32 num = 0; num < nr_devs; num++) {
        probe(make_dev(driver, num));
    }
    return 0;
}
