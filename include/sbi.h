#pragma once

#include "time.h"
#include "types.h"

#define EID_TIME 0x54494D45
#define EID_SRST 0x53525354

isize sbi_ecall(i32 eid, i32 fid, ...);

static inline i32 sbi_set_timer(time_t stime_value) {
    return sbi_ecall(EID_TIME, 0, stime_value);
}
static inline i32 sbi_system_reset(uint32_t reset_type, uint32_t reset_reason) {
    return sbi_ecall(EID_SRST, 0, reset_type, reset_reason);
}
