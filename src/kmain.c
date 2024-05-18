#include <kalloc.h>
#include <page_alloc.h>
#include <plic.h>
#include <sched.h>
#include <timer.h>
#include <trap.h>
#include <tty.h>

void init(void);

static timer_t watchdog_timer;

void watchdog_reset(void *data) {
    (void) data;
    *((volatile u32 *) 0x06011010) = 0x000014AF;
    timer_wait(&watchdog_timer, 5 * NS_IN_SEC, watchdog_reset, nullptr);
}

void init_watchdog(void) {
    *((volatile u32 *) 0x06011014) = 0x16AA0001;
    *((volatile u32 *) 0x06011018) = 0x16AA0081;

    timer_init(&watchdog_timer);
    timer_wait(&watchdog_timer, 5 * NS_IN_SEC, watchdog_reset, nullptr);
}

static void init_subsystems(void) {
    init_page_alloc();
    init_plic();
    init_trap();
    init_tty();
    init_timer();
    init_watchdog();
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
