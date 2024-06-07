#include <drivers/fb.h>

#include <config.h>
#include <errno.h>
#include <init.h>

typedef struct {
    dev_t dev;
} ctrl_blk_t;

static ctrl_blk_t ctrl_blks[DRIVER_DEV_CAPACITY];
static void (*hooks[FB_DEV_HOOK_CAPACITY])(u32 num, dev_t dev);
static u32 nr_dev_hooks;

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
    u32 num = fb_driver.nr_devs++;
    ctrl_blks[num].dev = dev;
    for (u32 i = 0; i < nr_dev_hooks; i++) {
        hooks[i](num, dev);
    }
    return 0;
}

i32 fb_register_dev_hook(void (*hook)(u32 num, dev_t dev)) {
    if (nr_dev_hooks == FB_DEV_HOOK_CAPACITY)
        return -EAGAIN;
    hooks[nr_dev_hooks++] = hook;
    return 0;
}
