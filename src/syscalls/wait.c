#include <syscall.h>

#include <errno.h>
#include <kalloc.h>
#include <sched.h>

isize sys_wait(const trapframe_t *frame) {
    i32 *status = (i32 *) frame->a0;

    if (list_empty(&curr_proc->children) &&
        list_empty(&curr_proc->zombie_children))
        return -ECHILD;

    proc_t *proc;
    if (!list_empty(&curr_proc->zombie_children)) {
        proc = list_first_entry(&curr_proc->zombie_children, proc_t, tree_node);
        goto quit;
    }

    sched_update_state(curr_proc, PROC_STATE_WAIT_CHILD);
    proc = curr_proc->proc_waiting;

quit:
    if (status)
        *status = proc->exit_status & 0xFF;
    proc_deinit(proc);
    kfree(proc);
    return proc->pid;
}
