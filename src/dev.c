#include <dev.h>

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
