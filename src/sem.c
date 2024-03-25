#include <sem.h>

#include <sched.h>

void sem_init(sem_t *sem) {
    list_init_head(&sem->wait_queue);
    sem->val = 0;
}

void sem_deinit(sem_t *sem) {
    while (sem->val++ < 0) {
        proc_t *proc = list_first_entry(&sem->wait_queue, proc_t, node);
        list_pop_front(&sem->wait_queue);
        sched_update_state(proc, PROC_STATE_READY);
    }
}

void sem_wait(sem_t *sem) {
    if (--sem->val < 0) {
        list_push_back(&sem->wait_queue, &curr_proc->node);
        sched_update_state(curr_proc, PROC_STATE_WAIT_SEM);
    }
}

void sem_signal(sem_t *sem) {
    if (sem->val++ < 0) {
        proc_t *proc = list_first_entry(&sem->wait_queue, proc_t, node);
        list_pop_front(&sem->wait_queue);
        sched_update_state(proc, PROC_STATE_READY);
    }
}

void sem_signaln(sem_t *sem, usize n) {
    for (usize i = 0; i < n; i++) {
        sem_signal(sem);
    }
}
