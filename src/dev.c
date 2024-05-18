#include <dev.h>

#include <driver.h>
#include <errno.h>

i32 dev_getc(dev_t dev) {
    u32 major = dev_major(dev);
    u32 minor = dev_minor(dev);
    if (major >= nr_drivers || minor >= driver_table[major]->nr_devs ||
        !driver_table[major]->getc)
        return -ENXIO;
    return driver_table[major]->getc(minor);
}

i32 dev_putc(dev_t dev, char c) {
    u32 major = dev_major(dev);
    u32 minor = dev_minor(dev);
    if (major >= nr_drivers || minor >= driver_table[major]->nr_devs ||
        !driver_table[major]->putc)
        return -ENXIO;
    return driver_table[major]->putc(minor, c);
}
