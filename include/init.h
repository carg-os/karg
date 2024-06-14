#pragma once

#include <compiler.h>
#include <types.h>

typedef struct {
    const char *name;
    i32 (*init)(void);
} init_t;

#define module_pre_init(func)                                                  \
    section(.init.pre_init)                                                    \
        used static const init_t _PRE_INIT = {_MODULE_NAME, func}
#define module_init(func)                                                      \
    section(.init.init) used static const init_t _INIT = {_MODULE_NAME, func}
#define module_post_init(func)                                                 \
    section(.init.post_init)                                                   \
        used static const init_t _POST_INIT = {_MODULE_NAME, func}

i32 init_modules(void);
