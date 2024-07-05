#include <errno.h>
#include <module/init.h>
#include <module/log.h>
#include <module/module.h>
#include <sched.h>

void init(void);

MODULE_NAME("kmain");

static proc_t init_proc;

[[noreturn]] void kmain(void) {
    init_modules();
    i32 res = proc_init(&init_proc, init, 0, nullptr, 0, nullptr);
    if (res < 0)
        log_panic("errno %s is returned when initializing init process",
                  errno_name(-res));
    sched_update_state(&init_proc, PROC_STATE_READY);
    sched_start();
}
