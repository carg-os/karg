#pragma once

#include <csr.h>
#include <types.h>

#define NS_IN_SEC 1000000000ULL
#define TIME_MAX U64_MAX

typedef u64 time_t;

static inline time_t time(void) { return csr_read(time); }
