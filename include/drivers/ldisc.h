#pragma once

#include <config.h>
#include <dev.h>

typedef struct {
    bool crlf;
} ldisc_config_t;

typedef struct {
    dev_t dev;
    ldisc_config_t config;
} ldisc_dev_t;

typedef struct {
    ldisc_dev_t src;
    ldisc_dev_t sinks[TTY_SINK_CAPACITY];
    u32 nr_sinks;
    u32 cursor_pos;
} ldisc_ctrl_blk_t;

i32 ldisc_register_src(u32 num, const ldisc_dev_t *dev);
i32 ldisc_register_sink(u32 num, const ldisc_dev_t *dev);
void ldisc_recv_byte(u32 num, u8 byte);
isize ldisc_write(u32 num, const u8 *buf, usize size);
