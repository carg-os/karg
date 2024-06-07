#pragma once

#include <compiler.h>
#include <types.h>

#define MODULE_PRE_INIT(func)                                                  \
    __section(".init.pre_init") __used static i32 (*_pre_init)(void) = func
#define MODULE_INIT(func)                                                      \
    __section(".init.init") __used static i32 (*_init)(void) = func
#define MODULE_POST_INIT(func)                                                 \
    __section(".init.post_init") __used static i32 (*_post_init)(void) = func

i32 init_modules(void);
