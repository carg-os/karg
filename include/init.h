#pragma once

#include <compiler.h>
#include <types.h>

#define MODULE_PRE_INIT(init)                                                  \
    __section(.init.pre_init) __used static i32 (*_pre_init)(void) = init
#define MODULE_INIT(init)                                                      \
    __section(.init.init) __used static i32 (*_init)(void) = init
#define MODULE_POST_INIT(init)                                                 \
    __section(.init.post_init) __used static i32 (*_post_init)(void) = init

i32 init_modules(void);
