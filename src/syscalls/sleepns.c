#include <syscall.h>

#include <sched.h>
#include <timer.h>

static void wakeup_proc(void *data) {
    sched_update_state((proc_t *) data, PROC_STATE_READY);
}

isize sys_sleepns(const trapframe_t *frame) {
    time_t ns = frame->a0;

    timer_wait(&curr_proc->timer, ns, wakeup_proc, curr_proc);
    sched_update_state(curr_proc, PROC_STATE_SLEEP);
    return 0;
}
