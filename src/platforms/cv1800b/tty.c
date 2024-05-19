#include <tty.h>

#include <uart.h>

i32 (*const TTY_INITS[])(void) = {
    init_uart,
    nullptr,
};

const dev_t TTY_SRC = DEV_INIT(1, 0);

const dev_t TTY_SINKS[] = {
    DEV_INIT(1, 0),
    DEV_INIT(0, 0),
};
