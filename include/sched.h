#ifndef SCHED_H_
#define SCHED_H_

#include <proc.h>

extern proc_t *curr_proc;

i32 init_sched(void);
void sched_update_state(proc_t *proc, proc_state_t state);
void sched_start(void);
void sched_resched(void);

#endif // SCHED_H_
