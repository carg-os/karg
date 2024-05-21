#include <syscall.h>

#include <sched.h>

isize sys_getpid(const trapframe_t *frame) {
    (void) frame;
    return curr_proc->pid;
}
