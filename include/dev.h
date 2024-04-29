#ifndef DEV_H_
#define DEV_H_

#include <errno.h>
#include <types.h>

typedef u32 dev_t;

static inline dev_t dev_init(u32 major, u32 minor) {
    return major << 20 | minor;
}

static inline u32 dev_major(dev_t dev) { return dev & 0xFFF00000; }

static inline u32 dev_minor(dev_t dev) { return dev & 0x000FFFFF; }

typedef struct {
    i32 (*getc)(u32 minor);
    i32 (*putc)(u32 minor, char c);
} driver_t;

extern driver_t DRIVERS[];
extern const usize NR_DRIVERS;

static inline i32 dev_getc(dev_t dev) {
    u32 major = dev_major(dev);
    if (dev >= NR_DRIVERS || !DRIVERS[major].getc)
        return -ENXIO;
    return DRIVERS[dev].getc(dev_minor(dev));
}

static inline i32 dev_putc(dev_t dev, char c) {
    u32 major = dev_major(dev);
    if (dev >= NR_DRIVERS || !DRIVERS[major].putc)
        return -ENXIO;
    return DRIVERS[dev].putc(dev_minor(dev), c);
}

#endif // DEV_H_
