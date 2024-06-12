#include <syscall.h>

#include <sched.h>

[[noreturn]] isize sys_exit(const trapframe_t *frame) {
    i32 status = frame->a0;

    curr_proc->exit_status = status;

    while (!list_empty(&curr_proc->children)) {
        proc_t *child =
            list_first_entry(&curr_proc->children, proc_t, tree_node);
        proc_adopt(proc_table[1], child);
    }

    while (!list_empty(&curr_proc->zombie_children)) {
        proc_t *child =
            list_first_entry(&curr_proc->zombie_children, proc_t, tree_node);
        proc_adopt(proc_table[1], child);
    }

    proc_t *parent = curr_proc->parent;
    list_remove(&curr_proc->tree_node);
    list_push_back(&parent->zombie_children, &curr_proc->tree_node);

    if (parent->state == PROC_STATE_WAIT_CHILD) {
        parent->proc_waiting = curr_proc;
        sched_update_state(parent, PROC_STATE_READY);
    }

    sched_update_state(curr_proc, PROC_STATE_ZOMBIE);
    unreachable();
}
