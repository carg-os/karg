#pragma once

#include <compiler.h>
#include <types.h>

#define module_pre_init(func)                                                  \
    __section(.init.pre_init) __used static i32 (*_pre_init)(void) = func
#define module_init(func)                                                      \
    __section(.init.init) __used static i32 (*_init)(void) = func
#define module_post_init(func)                                                 \
    __section(.init.post_init) __used static i32 (*_post_init)(void) = func

i32 init_modules(void);
