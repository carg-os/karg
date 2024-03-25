#ifndef TIME_H_
#define TIME_H_

#include <csr.h>
#include <limits.h>
#include <types.h>

#define NS_IN_SEC 1000000000

typedef u64 time_t;
#define TIME_MAX UINT64_MAX

static inline time_t time(void) { return csr_read(time); }

#endif // TIME_H_
