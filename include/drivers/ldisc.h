#pragma once

#include <config.h>
#include <dev.h>

typedef struct {
    dev_t src;
    dev_t sinks[TTY_SINK_CAPACITY];
    u32 nr_sinks;
    u32 cursor_pos;
} ldisc_ctrl_blk_t;

i32 ldisc_register_src(u32 num, dev_t dev);
i32 ldisc_register_sink(u32 num, dev_t dev);
void ldisc_recv_byte(u32 num, u8 byte);
isize ldisc_write(u32 num, const u8 *buf, usize size);
