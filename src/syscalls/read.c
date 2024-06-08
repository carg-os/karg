#include <syscall.h>

#include <errno.h>
#include <sched.h>

isize sys_read(const trapframe_t *frame) {
    i32 fd = frame->a0;
    u8 *buf = (u8 *) frame->a1;
    usize size = frame->a2;
    if (proc_is_bad_fd(curr_proc, fd))
        return -EBADF;
    if (!(curr_proc->fds[fd].flags & FD_FLAG_READABLE))
        return -EINVAL;
    if (!buf)
        return -EFAULT;

    return dev_read(curr_proc->fds[fd].dev, buf, size);
}
