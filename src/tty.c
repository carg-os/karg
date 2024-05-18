#include <tty.h>

#include <dev.h>
#include <driver.h>
#include <fb.h>
#include <uart.h>

i32 tty_getc(u32 minor);
i32 tty_putc(u32 minor, char c);

static driver_t driver = {
    .nr_devs = 1,
    .irqs = nullptr,
    .isr = nullptr,
    .getc = tty_getc,
    .putc = tty_putc,
};

i32 init_tty(void) {
    i32 res;

    res = driver_add(&driver);
    if (res < 0)
        return res;

    res = init_uart();
    if (res < 0)
        return res;

    res = init_fb();
    if (res < 0)
        return res;

    return 0;
}

i32 tty_getc(u32 minor) {
    (void) minor;

    return dev_getc(dev_init(1, 0));
}

i32 tty_putc(u32 minor, char c) {
    (void) minor;

    i32 res;

    res = dev_putc(dev_init(1, 0), c);
    if (res < 0)
        return res;

    res = dev_putc(dev_init(2, 0), c);
    if (res < 0)
        return res;

    return 0;
}
