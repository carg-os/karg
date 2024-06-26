#pragma once

#include <types.h>

static inline i32 strcmp(const char *lhs, const char *rhs) {
    while (*lhs && *lhs == *rhs) {
        lhs++;
        rhs++;
    }
    return *lhs - *rhs;
}

static inline usize strlen(const char *str) {
    usize len = 0;
    while (*str) {
        str++;
        len++;
    }
    return len;
}
