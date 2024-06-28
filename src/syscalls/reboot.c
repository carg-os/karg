#include <syscall.h>

#include <errno.h>
#include <pm.h>

typedef enum {
    REBOOT_TYPE_REBOOT = 0,
    REBOOT_TYPE_SHUTDOWN = 1,
    REBOOT_TYPE_HANG = 2,
} reboot_type_t;

isize sys_reboot(const trapframe_t *frame) {
    reboot_type_t type = frame->a0;

    switch (type) {
    case REBOOT_TYPE_REBOOT:
        pm_reboot();
    case REBOOT_TYPE_SHUTDOWN:
        pm_shutdown();
    case REBOOT_TYPE_HANG:
        pm_hang();
    default:
        return -EINVAL;
    }
}
