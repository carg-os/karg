#include <dev.h>

#include <errno.h>
#include <uart.h>

driver_t DRIVERS[] = {
    (driver_t){
        .getc = uart_getc,
        .putc = uart_putc,
    },
};
const usize NR_DRIVERS = sizeof(DRIVERS) / sizeof(DRIVERS[0]);

i32 dev_getc(dev_t dev) {
    u32 major = dev_major(dev);
    if (dev >= NR_DRIVERS || !DRIVERS[major].getc)
        return -ENXIO;
    return DRIVERS[major].getc(dev_minor(dev));
}

i32 dev_putc(dev_t dev, char c) {
    u32 major = dev_major(dev);
    if (dev >= NR_DRIVERS || !DRIVERS[major].putc)
        return -ENXIO;
    return DRIVERS[major].putc(dev_minor(dev), c);
}
