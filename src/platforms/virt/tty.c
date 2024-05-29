#include <tty.h>

i32 init_uart(void);
i32 init_fbcon(void);

i32 (*const TTY_INITS[])(void) = {
    init_uart,
    init_fbcon,
    nullptr,
};

const dev_t TTY_SRC = (dev_t){.driver = &uart_driver, .num = 0};

const dev_t TTY_SINKS[] = {
    (dev_t){.driver = &uart_driver, .num = 0},
    (dev_t){.driver = &fbcon_driver, .num = 0},
    (dev_t){.driver = nullptr, .num = 0},
};
