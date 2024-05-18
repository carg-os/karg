#pragma once

#include <time.h>
#include <types.h>

typedef struct {
    isize err;
    isize val;
} sbi_res_t;

sbi_res_t sbi_ecall(i32 eid, i32 fid, ...);
i32 sbi_map_err(isize err);

#define EID_TIME 0x54494D45

static inline i32 sbi_set_timer(time_t time) {
    return sbi_map_err(sbi_ecall(EID_TIME, 0, time).err);
}

#define EID_SRST 0x53525354

typedef enum {
    SBI_REBOOT_TYPE_SHUTDOWN = 0,
    SBI_REBOOT_TYPE_REBOOT = 1,
} sbi_reboot_type_t;

typedef enum {
    SBI_REBOOT_REASON_NONE = 0,
} sbi_reboot_reason_t;

static inline i32 sbi_reboot(sbi_reboot_type_t type,
                             sbi_reboot_reason_t reason) {
    return sbi_map_err(sbi_ecall(EID_SRST, 0, type, reason).err);
}
