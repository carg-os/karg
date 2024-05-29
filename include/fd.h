#pragma once

#include <dev.h>

#define FD_FLAG_ALLOCATED 0x00000001
#define FD_FLAG_READABLE 0x00000002
#define FD_FLAG_WRITABLE 0x00000004

typedef struct {
    u32 flags;
    dev_t dev;
} fd_t;

bool is_bad_fd(isize fd);
