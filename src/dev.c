#include <dev.h>

#include <init.h>
#include <log.h>
#include <module.h>
#include <string.h>

MODULE_NAME("dev");

extern dev_init_t _dev_init_start, _dev_init_end;

isize dev_read(dev_t dev, u8 *buf, usize size) {
    return dev.driver->read(dev.num, buf, size);
}

isize dev_write(dev_t dev, const u8 *buf, usize size) {
    return dev.driver->write(dev.num, buf, size);
}

i32 dev_ioctl(dev_t dev, u32 req, ...) {
    va_list args;
    va_start(args);
    i32 res = dev.driver->ioctl(dev.num, req, args);
    va_end(args);
    return res;
}

i32 dev_vioctl(dev_t dev, u32 req, va_list args) {
    return dev.driver->ioctl(dev.num, req, args);
}

static i32 init(void) {
    for (usize i = 0; DEV_TABLE[i].name; i++) {
        const dev_node_t *node = &DEV_TABLE[i];
        for (dev_init_t *init_ptr = &_dev_init_start; init_ptr < &_dev_init_end;
             init_ptr++) {
            if (strcmp(init_ptr->compat, node->name) == 0) {
                i32 res = init_ptr->init(node);
                log_info("loaded driver %s for device %s", init_ptr->name,
                         init_ptr->compat);
                if (res < 0)
                    return res;
                break;
            }
        }
    }
    return 0;
}

module_init(init);
