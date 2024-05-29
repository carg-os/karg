#include <dev.h>

#include <driver.h>
#include <errno.h>

isize dev_read(dev_t dev, u8 *buf, usize size) {
    if (dev.num >= dev.driver->nr_devs || !dev.driver->read)
        return -ENXIO;
    return dev.driver->read(dev.num, buf, size);
}

isize dev_write(dev_t dev, const u8 *buf, usize size) {
    if (dev.num >= dev.driver->nr_devs || !dev.driver->write)
        return -ENXIO;
    return dev.driver->write(dev.num, buf, size);
}
