#include <drivers/fb.h>

#include <config.h>
#include <errno.h>

typedef struct {
    dev_t dev;
} ctrl_blk_t;

static ctrl_blk_t ctrl_blks[FB_DEV_CAPACITY];

static i32 ioctl(u32 num, u32 req, va_list args) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    return dev_vioctl(ctrl_blk->dev, req, args);
}

driver_t fb_driver = {
    .nr_devs = 0,
    .read = nullptr,
    .write = nullptr,
    .ioctl = ioctl,
};

i32 init_fb(void) { return 0; }

i32 fb_register_dev(dev_t dev) {
    if (fb_driver.nr_devs == FB_DEV_CAPACITY)
        return -EAGAIN;
    ctrl_blks[fb_driver.nr_devs++].dev = dev;
    return 0;
}
