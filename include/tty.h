#pragma once

#include <dev.h>
#include <types.h>

extern i32 (*const TTY_INITS[])(void);
extern const dev_t TTY_SRC;
extern const dev_t TTY_SINKS[];

i32 init_tty(void);
