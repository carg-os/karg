#pragma once

#include <dev.h>
#include <types.h>

i32 tty_register_src(u32 num, dev_t src);
i32 tty_register_sink(u32 num, dev_t sink);

extern
