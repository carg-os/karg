#include <pm.h>

#include <arch/riscv/sbi.h>
#include <stddef.h>

[[noreturn]] void pm_reboot(void) {
    sbi_reboot(SBI_REBOOT_TYPE_REBOOT, SBI_REBOOT_REASON_NONE);
    unreachable();
}

[[noreturn]] void pm_shutdown(void) {
    sbi_reboot(SBI_REBOOT_TYPE_SHUTDOWN, SBI_REBOOT_REASON_NONE);
    unreachable();
}

[[noreturn]] void pm_hang(void) {
    while (true) {
        asm volatile("wfi");
    }
}
