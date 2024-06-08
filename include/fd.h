#pragma once

#include <dev.h>
#include <types.h>

#define FD_FLAG_USED 0x1
#define FD_FLAG_READABLE 0x2
#define FD_FLAG_WRITABLE 0x4

typedef struct {
    u32 flags;
    dev_t dev;
} fd_t;
