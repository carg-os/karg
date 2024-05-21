#include <syscall.h>

isize (*const SYSCALL_TABLE[])(const trapframe_t *) = {
    sys_exit,   sys_read, sys_write, sys_yield,  sys_sleepns,
    sys_getpid, sys_proc, sys_wait,  sys_reboot,
};
const usize NR_SYSCALLS = sizeof(SYSCALL_TABLE) / sizeof(SYSCALL_TABLE[0]);
