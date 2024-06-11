#pragma once

#include <types.h>

#define NS_IN_SEC 1000000000U

typedef u64 time_t;

#define TIME_MAX U64_MAX

time_t time(void);
