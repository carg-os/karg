#include <pm.h>

#include <arch/riscv/sbi.h>
#include <errno.h>
#include <module/log.h>
#include <module/module.h>

MODULE_NAME("pm");

[[noreturn]] void pm_reboot(void) {
    i32 res = sbi_reboot(SBI_REBOOT_TYPE_REBOOT, SBI_REBOOT_REASON_NONE);
    log_panic("errno %s is returned when rebooting system", errno_name(-res));
}

[[noreturn]] void pm_shutdown(void) {
    i32 res = sbi_reboot(SBI_REBOOT_TYPE_SHUTDOWN, SBI_REBOOT_REASON_NONE);
    log_panic("errno %s is returned when shutting down system",
              errno_name(-res));
}
