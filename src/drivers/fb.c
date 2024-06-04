#include <drivers/fb.h>

typedef struct {
    dev_t sinks[FB_MAX_SINK + 1];
    u32 nr_sinks;
} fb_dev_t;

driver_t fb_driver = {
    .nr_devs = FB_NR_DEVS,
    .read = nullptr,
    .write = nullptr,
};

i32 init_fb(void) { return 0; }

i32 fb_register_sink(u32 num, dev_t sink) {
    if (num >= FB_NR_DEVS)
        return -ENXIO;
    fb_dev_t *dev = fb_devs[num];
    dev->sinks[dev->nr_devs++] = sink;
}

i32 fb_flush(u32 y, u32 height) {}
