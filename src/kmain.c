#include <kalloc.h>
#include <page_alloc.h>
#include <plic.h>
#include <sched.h>
#include <timer.h>
#include <trap.h>
#include <uart.h>

extern void init(void);

static void init_subsystems(void) {
    init_page_alloc();

    init_plic();
    init_trap();

    init_uart();
    init_timer();

    init_sched();
}

[[noreturn]] void kmain(void) {
    init_subsystems();

    proc_t *init_proc = (proc_t *) kmalloc(sizeof(proc_t));
    proc_init(init_proc, init, 0, nullptr, 0, nullptr);

    sched_update_state(init_proc, PROC_STATE_READY);
    sched_start();

    unreachable();
}
