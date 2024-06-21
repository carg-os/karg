#include <syscall.h>

#include <errno.h>
#include <hang.h>
#include <sbi.h>

typedef enum {
    REBOOT_TYPE_REBOOT = 0,
    REBOOT_TYPE_SHUTDOWN = 1,
    REBOOT_TYPE_HANG = 2,
} reboot_type_t;

isize sys_reboot(const trapframe_t *frame) {
    reboot_type_t type = frame->a0;

    switch (type) {
    case REBOOT_TYPE_REBOOT:
        return sbi_reboot(SBI_REBOOT_TYPE_REBOOT, SBI_REBOOT_REASON_NONE);
    case REBOOT_TYPE_SHUTDOWN:
        return sbi_reboot(SBI_REBOOT_TYPE_SHUTDOWN, SBI_REBOOT_REASON_NONE);
    case REBOOT_TYPE_HANG:
        hang();
    default:
        return -EINVAL;
    }
}
