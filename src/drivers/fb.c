#include <drivers/fb.h>

#include <errno.h>
#include <platform.h>

typedef struct {
    dev_t dev;
} ctrl_blk_t;

static ctrl_blk_t ctrl_blks[FB_NR_DEVS];
static u32 nr_devs = 0;

i32 init_fb(void) { return 0; }

i32 fb_register_dev(dev_t dev) {
    if (nr_devs >= FB_NR_DEVS)
        return -EAGAIN;
    ctrl_blks[nr_devs++].dev = dev;
    return 0;
}

i32 fb_get_size(u32 num, u32 *width, u32 *height) {
    if (num >= nr_devs)
        return -ENXIO;
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    return dev_ioctl(ctrl_blk->dev, 0, width, height);
}

i32 fb_write_pix(u32 num, u32 x, u32 y, u32 val) {
    if (num >= nr_devs)
        return -ENXIO;
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    return dev_ioctl(ctrl_blk->dev, 1, x, y, val);
}

i32 fb_flush(u32 num, u32 y, u32 height) {
    if (num >= nr_devs)
        return -ENXIO;
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    return dev_ioctl(ctrl_blk->dev, 2, y, height);
}
