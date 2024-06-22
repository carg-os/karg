#pragma once

#include <drivers/ldisc.h>
#include <sem.h>

typedef struct {
    ldisc_ctrl_blk_t ldisc_ctrl_blk;
    u8 *rx_buf;
    usize rx_head, rx_tail;
    sem_t rx_sem;
} tty_ctrl_blk_t;

extern tty_ctrl_blk_t *tty_ctrl_blks[DRIVER_DEV_CAPACITY];

i32 tty_init_ctrl_blk(tty_ctrl_blk_t *ctrl_blk);
