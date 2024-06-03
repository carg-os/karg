#include <init.h>
#include <kalloc.h>
#include <sched.h>

void init(void);

[[noreturn]] void kmain(void) {
    init_modules();

    proc_t *init_proc = (proc_t *) kmalloc(sizeof(proc_t));
    proc_init(init_proc, init, 0, nullptr, 0, nullptr);

    sched_update_state(init_proc, PROC_STATE_READY);

    sched_start();

    unreachable();
}
