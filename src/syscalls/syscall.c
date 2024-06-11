#include <syscall.h>

isize sys_exit(const trapframe_t *frame);
isize sys_read(const trapframe_t *frame);
isize sys_write(const trapframe_t *frame);
isize sys_yield(const trapframe_t *frame);
isize sys_sleep(const trapframe_t *frame);
isize sys_getpid(const trapframe_t *frame);
isize sys_proc(const trapframe_t *frame);
isize sys_wait(const trapframe_t *frame);
isize sys_reboot(const trapframe_t *frame);

isize (*const SYSCALL_TABLE[])(const trapframe_t *) = {
    sys_exit,   sys_read, sys_write, sys_yield,  sys_sleep,
    sys_getpid, sys_proc, sys_wait,  sys_reboot,
};
const usize NR_SYSCALLS = sizeof(SYSCALL_TABLE) / sizeof(SYSCALL_TABLE[0]);
