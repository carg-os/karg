#include <timer.h>

#include <arch/riscv/csr.h>
#include <arch/riscv/sbi.h>
#include <module/init.h>
#include <module/module.h>
#include <timer.h>

MODULE_NAME("timer");

static i32 init(void) {
    csr_set_bits(sie, CSR_SIE_STIE);
    return sbi_set_timer(TIME_MAX);
}

module_pre_init(init);

void timer_set(time_t wait_time) { sbi_set_timer(wait_time); }
