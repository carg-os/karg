#include <fd.h>

#include <config.h>
#include <sched.h>

bool is_bad_fd(i32 fd) {
    return fd < 0 || fd >= PROC_FD_CAPACITY ||
           !(curr_proc->fds[fd].flags & FD_FLAG_ALLOCATED);
}
