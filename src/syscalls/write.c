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

    for (usize i = 0; i < size; i++) {
        i32 res = dev_putc(curr_proc->fds[fd].dev, buf[i]);
        if (res < 0)
            return res;
    }
    return size;
}
