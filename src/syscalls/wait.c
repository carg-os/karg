#include <syscall.h>

#include <errno.h>
#include <kalloc.h>
#include <sched.h>

isize sys_wait(const trapframe_t *frame) {
    i32 *status = (i32 *) frame->a0;

    if (list_empty(&curr_proc->children) &&
        list_empty(&curr_proc->zombie_children))
        return -ECHILD;

    if (!list_empty(&curr_proc->zombie_children)) {
        proc_t *proc =
            list_first_entry(&curr_proc->zombie_children, proc_t, tree_node);
        if (status)
            *status = proc->exit_status & 0xFF;
        proc_deinit(proc);
        kfree(proc);
        return proc->pid;
    }

    sched_update_state(curr_proc, PROC_STATE_WAIT_CHILD);

    proc_t *proc = curr_proc->proc_waiting;
    if (status)
        *status = proc->exit_status & 0xFF;
    proc_deinit(proc);
    kfree(proc);
    return proc->pid;
}
