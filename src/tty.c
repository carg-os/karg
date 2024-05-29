#include <tty.h>

#include <dev.h>
#include <driver.h>

isize tty_read(u32 minor, u8 *buf, usize size) {
    (void) minor;

    return dev_read(TTY_SRC, buf, size);
}

isize tty_write(u32 minor, const u8 *buf, usize size) {
    (void) minor;

    for (usize i = 0; TTY_SINKS[i].driver; i++) {
        isize res = dev_write(TTY_SINKS[i], buf, size);
        if (res < 0)
            return res;
    }

    return 0;
}

driver_t tty_driver = {
    .nr_devs = 1,
    .irqs = nullptr,
    .isr = nullptr,
    .read = tty_read,
    .write = tty_write,
};

i32 init_tty(void) {
    for (usize i = 0; TTY_INITS[i]; i++) {
        i32 res = TTY_INITS[i]();
        if (res < 0)
            return res;
    }

    return 0;
}
