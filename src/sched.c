#include <sched.h>

#include <arch.h>
#include <module/init.h>
#include <module/module.h>

MODULE_NAME("sched");

static list_node_t ready_queue = list_head_init(ready_queue);
static proc_t idle_proc;
static timer_t timer;

proc_t *curr_proc = nullptr;

static void idle_task() {
    while (true) {
        wait_for_intr();
    }
}

static i32 init(void) {
    i32 res =
        proc_init(&idle_proc, idle_task, PROC_FLAG_KERN, nullptr, 0, nullptr);
    if (res < 0)
        return res;
    sched_update_state(&idle_proc, PROC_STATE_READY);

    timer_init(&timer);

    return 0;
}

module_post_init(init);

void sched_update_state(proc_t *proc, proc_state_t state) {
    if (proc->state == state)
        return;

    switch (proc->state) {
    case PROC_STATE_READY:
        list_remove(&proc->node);
        break;
    case PROC_STATE_SLEEP:
        timer_cancel(&proc->timer);
        break;
    default:
        break;
    }

    proc->state = state;

    switch (state) {
    case PROC_STATE_READY:
        list_push_front(&ready_queue, &proc->node);
        break;
    case PROC_STATE_CURR:
        curr_proc = proc;
        break;
    case PROC_STATE_WAIT_SEM:
    case PROC_STATE_WAIT_CHILD:
    case PROC_STATE_SLEEP:
        if (proc == curr_proc)
            sched_resched();
        break;
    case PROC_STATE_ZOMBIE:
        if (proc == curr_proc) {
            curr_proc = nullptr;
            sched_resched();
        }
        break;
    default:
        break;
    }
}

static void sched_preempt(void *data) {
    (void) data;
    timer_wait(&timer, SCHED_TIMESLICE, sched_preempt, nullptr);
    sched_resched();
}

[[noreturn]] void sched_start(void) {
    timer_wait(&timer, SCHED_TIMESLICE, sched_preempt, nullptr);
    sched_resched();
    unreachable();
}

void sched_resched(void) {
    proc_t *old_proc = curr_proc;
    if (old_proc && old_proc->state == PROC_STATE_CURR)
        sched_update_state(old_proc, PROC_STATE_READY);

    proc_t *new_proc = list_first_entry(&ready_queue, proc_t, node);
    sched_update_state(new_proc, PROC_STATE_CURR);

    proc_ctx_sw(old_proc, new_proc);
}
