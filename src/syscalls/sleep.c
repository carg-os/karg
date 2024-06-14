#include <syscall.h>

#include <sched.h>
#include <time.h>

static void wakeup_proc(void *data) {
    sched_update_state((proc_t *) data, PROC_STATE_READY);
}

isize sys_sleep(const trapframe_t *frame) {
    time_t ns = frame->a0;

    timer_wait(&curr_proc->timer, ns, wakeup_proc, curr_proc);
    sched_update_state(curr_proc, PROC_STATE_SLEEP);
    return 0;
}
