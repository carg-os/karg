#pragma once

#include <proc.h>

extern proc_t *curr_proc;

void sched_update_state(proc_t *proc, proc_state_t state);
[[noreturn]] void sched_start(void);
void sched_resched(void);
