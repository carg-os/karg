#include <module/init.h>
#include <sched.h>

void init(void);

static proc_t init_proc;

[[noreturn]] void kmain(void) {
    init_modules();

    proc_config_t proc_config;
    proc_config.entry = init;
    proc_config.flags = 0;
    proc_config.parent = nullptr;
    proc_config.argc = 0;
    proc_config.argv = nullptr;
    proc_init(&init_proc, &proc_config);
    sched_update_state(&init_proc, PROC_STATE_READY);

    sched_start();

    unreachable();
}
