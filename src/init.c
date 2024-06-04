#include <init.h>

#include <drivers/fb.h>
#include <drivers/fbcon.h>
#include <drivers/tty.h>
#include <drivers/uart.h>
#include <drivers/virtio-gpu.h>
#include <page_alloc.h>
#include <plic.h>
#include <sched.h>
#include <timer.h>
#include <trap.h>

static i32 (*const pre_init[])() = {
    init_page_alloc,
    init_plic,
    init_trap,
};

static i32 (*const init[])() = {
    init_fb,
    init_timer,
    init_tty,
};

static i32 (*const post_init[])() = {
    init_uart,
    init_fbcon,
    init_sched,
    init_virtio_gpu,
};

i32 init_modules(void) {
    for (usize i = 0; i < sizeof(pre_init) / sizeof(pre_init[0]); i++) {
        i32 res = pre_init[i]();
        if (res < 0)
            return res;
    }

    for (usize i = 0; i < sizeof(init) / sizeof(init[0]); i++) {
        i32 res = init[i]();
        if (res < 0)
            return res;
    }

    for (usize i = 0; i < sizeof(post_init) / sizeof(post_init[0]); i++) {
        i32 res = post_init[i]();
        if (res < 0)
            return res;
    }

    return 0;
}
