#pragma once

#include <dev.h>
#include <types.h>

i32 init_fb(void);
i32 fb_register_dev(dev_t dev);
i32 fb_get_size(u32 num, u32 *width, u32 *height);
i32 fb_write_pix(u32 num, u32 x, u32 y, u32 val);
i32 fb_flush(u32 num, u32 y, u32 height);
