#include <syscall.h>

#include <sched.h>

isize sys_yield(const trapframe_t *frame) {
    (void) frame;
    sched_resched();
    return 0;
}
