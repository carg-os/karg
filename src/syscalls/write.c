#include <syscall.h>

#include <errno.h>
#include <sched.h>

isize sys_write(const trapframe_t *frame) {
    isize fd = frame->a0;
    const u8 *buf = (const u8 *) frame->a1;
    usize size = frame->a2;
    if (is_bad_fd(fd))
        return -EBADF;
    if (!(curr_proc->fds[fd].flags & FD_FLAG_WRITABLE))
        return -EINVAL;
    if (!buf)
        return -EFAULT;

    return dev_write(curr_proc->fds[fd].dev, buf, size);
}
