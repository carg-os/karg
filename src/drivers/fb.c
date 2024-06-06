#include <drivers/fb.h>

#include <config.h>
#include <errno.h>
#include <init.h>

typedef struct {
    dev_t dev;
} ctrl_blk_t;

static ctrl_blk_t ctrl_blks[DRIVER_DEV_CAPACITY];

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

static i32 init(void) { return 0; }

MODULE_INIT(init);

i32 fb_register_dev(dev_t dev) {
    if (fb_driver.nr_devs == DRIVER_DEV_CAPACITY)
        return -EAGAIN;
    ctrl_blks[fb_driver.nr_devs++].dev = dev;
    return 0;
}
