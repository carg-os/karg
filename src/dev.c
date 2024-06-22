#include <dev.h>

#include <errno.h>
#include <init.h>
#include <log.h>
#include <module.h>
#include <string.h>

MODULE_NAME("dev");

extern dev_init_t _dev_init_start, _dev_init_end;

isize dev_read(dev_t dev, u8 *buf, usize size) {
    if (!dev.driver->read)
        return -ENXIO;
    return dev.driver->read(dev.num, buf, size);
}

isize dev_write(dev_t dev, const u8 *buf, usize size) {
    if (!dev.driver->write)
        return -ENXIO;
    return dev.driver->write(dev.num, buf, size);
}

i32 dev_ioctl(dev_t dev, u32 req, ...) {
    if (!dev.driver->ioctl)
        return -ENXIO;
    va_list args;
    va_start(args);
    i32 res = dev.driver->ioctl(dev.num, req, args);
    va_end(args);
    return res;
}

i32 dev_vioctl(dev_t dev, u32 req, va_list args) {
    if (!dev.driver->ioctl)
        return -ENXIO;
    return dev.driver->ioctl(dev.num, req, args);
}

static i32 init(void) {
    for (usize i = 0; DEV_TABLE[i].name; i++) {
        const dev_node_t *node = &DEV_TABLE[i];
        for (dev_init_t *dev_init = &_dev_init_start; dev_init < &_dev_init_end;
             dev_init++) {
            if (strcmp(dev_init->compat, node->name) == 0) {
                i32 res = dev_init->init(node);
                log_info("loaded driver %s for device %s", dev_init->name,
                         dev_init->compat);
                if (res < 0)
                    return res;
                break;
            }
        }
    }
    return 0;
}

module_init(init);
