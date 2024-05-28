#pragma once

#include <types.h>

extern u32 fb[];

i32 init_fb(u32 *width, u32 *height);
i32 fb_set_offset(u32 x, u32 y);
i32 fb_flush(u32 y, u32 height);
