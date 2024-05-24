#include <tty.h>

#include <uart.h>
#include <virtio.h>

i32 (*const TTY_INITS[])(void) = {
    init_uart,
    init_virtio,
    nullptr,
};

const dev_t TTY_SRC = DEV_INIT(1, 0);

const dev_t TTY_SINKS[] = {
    DEV_INIT(1, 0),
    DEV_INIT(2, 0),
    DEV_INIT(0, 0),
};
