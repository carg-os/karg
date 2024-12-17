#include <syscall.h>

#include <elf.h>
#include <errno.h>
#include <mm/vm.h>
#include <sched.h>

isize sys_write(const trapframe_t *frame) {
    i32 fd = frame->a0;
    usize buf_addr;
    i32 res = vm_virt_to_phys(curr_proc->page_table, frame->a1, &buf_addr);
    if (res < 0)
        return res;
    const u8 *buf = (const u8 *) buf_addr;
    usize size = frame->a2;
    if (proc_is_bad_fd(curr_proc, fd) &&
        !(curr_proc->fds[fd].flags & FD_FLAG_WRITABLE))
        return -EBADF;

    return dev_write(curr_proc->fds[fd].dev, buf, size);
}
