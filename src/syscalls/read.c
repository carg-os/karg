#include <syscall.h>

#include <errno.h>
#include <sched.h>

isize sys_read(const trapframe_t *frame) {
    isize fd = frame->a0;
    u8 *buf = (u8 *) frame->a1;
    usize size = frame->a2;
    if (is_bad_fd(fd))
        return -EBADF;
    if (!(curr_proc->fds[fd].flags & FD_FLAG_READABLE))
        return -EINVAL;
    if (!buf)
        return -EFAULT;

    for (usize i = 0; i < size; i++) {
        i32 res = dev_getc(curr_proc->fds[fd].dev);
        if (res < 0)
            return res;
        buf[i] = res;
        if (res == '\n')
            return i + 1;
    }
    return size;
}
