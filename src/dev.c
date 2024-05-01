#include <dev.h>

#include <driver.h>
#include <errno.h>

i32 dev_getc(dev_t dev) {
    u32 major = dev_major(dev);
    if (dev >= nr_drivers || !driver_table[major]->getc)
        return -ENXIO;
    return driver_table[major]->getc(dev_minor(dev));
}

i32 dev_putc(dev_t dev, char c) {
    u32 major = dev_major(dev);
    if (dev >= nr_drivers || !driver_table[major]->putc)
        return -ENXIO;
    return driver_table[major]->putc(dev_minor(dev), c);
}
