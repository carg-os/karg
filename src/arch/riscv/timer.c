#include <timer.h>

#include <arch/riscv/csr.h>
#include <arch/riscv/sbi.h>
#include <errno.h>
#include <module/init.h>
#include <module/log.h>
#include <module/module.h>

MODULE_NAME("timer");

static i32 init(void) {
    csr_set_bits(sie, CSR_SIE_STIE);
    return sbi_set_timer(TIME_MAX);
}

module_pre_init(init);

void timer_set(time_t wait_time) {
    i32 res = sbi_set_timer(wait_time);
    if (res < 0)
        log_panic("errno %s is returned when setting timer", errno_name(-res));
}
