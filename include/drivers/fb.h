#pragma once

#include <types.h>

typedef struct {
    u32 width, height;
    i32 (*write_pix)(u32 num, u32 x, u32 y, u32 val);
    i32 (*flush)(u32 y, u32 height);
} fb_dev_t;

i32 init_fb(void);
i32 fb_register_dev(fb_dev_t dev);
i32 fb_size(u32 num, u32 *width, u32 *height);
i32 fb_write_pix(u32 num, u32 x, u32 y, u32 val);
i32 fb_flush(u32 num, u32 y, u32 height);
