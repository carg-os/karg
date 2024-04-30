#pragma once

#include <csr.h>
#include <limits.h>
#include <types.h>

#define NS_IN_SEC 1000000000
#define TIME_MAX UINT64_MAX

typedef u64 time_t;

static inline time_t time(void) { return csr_read(time); }
