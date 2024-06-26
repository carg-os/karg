#include <module/init.h>
#include <sched.h>

void init(void);

static proc_t init_proc;

[[noreturn]] void kmain(void) {
    init_modules();

    proc_init(&init_proc, init, 0, nullptr, 0, nullptr);
    sched_update_state(&init_proc, PROC_STATE_READY);

    sched_start();

    unreachable();
}
