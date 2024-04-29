#include <fd.h>

#include <config.h>
#include <sched.h>

bool is_bad_fd(isize fd) {
    return fd < 0 || fd > PROC_MAX_FD ||
           !(curr_proc->fds[fd].flags & FD_FLAG_ALLOCATED);
}
