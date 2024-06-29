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

    proc_config_t proc_config;
    proc_config.entry = init;
    proc_config.flags = 0;
    proc_config.parent = nullptr;
    proc_config.argc = 0;
    proc_config.argv = nullptr;
    i32 res = proc_init(&init_proc, &proc_config);
    if (res < 0)
        log_panic("errno %s is returned when initializing init process",
                  errno_name(-res));
    sched_update_state(&init_proc, PROC_STATE_READY);

    sched_start();

    unreachable();
}
