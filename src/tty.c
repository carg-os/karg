#include <tty.h>

#include <dev.h>
#include <driver.h>
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
    i32 res = driver_add(&driver);
    if (res < 0)
        return res;

    for (usize i = 0; TTY_INITS[i]; i++) {
        res = TTY_INITS[i]();
        if (res < 0)
            return res;
    }

    return 0;
}

i32 tty_getc(u32 minor) {
    (void) minor;

    return dev_getc(TTY_SRC);
}

i32 tty_putc(u32 minor, char c) {
    (void) minor;

    for (usize i = 0; TTY_SINKS[i] != DEV_INIT(0, 0); i++) {
        i32 res = dev_putc(TTY_SINKS[i], c);
        if (res < 0)
            return res;
    }

    return 0;
}
