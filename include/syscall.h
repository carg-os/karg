#pragma once

#include <trap.h>

isize sys_exit(const trapframe_t *frame);
isize sys_getpid(const trapframe_t *frame);
isize sys_proc(const trapframe_t *frame);
isize sys_read(const trapframe_t *frame);
isize sys_reboot(const trapframe_t *frame);
isize sys_sleepns(const trapframe_t *frame);
isize sys_wait(const trapframe_t *frame);
isize sys_write(const trapframe_t *frame);
isize sys_yield(const trapframe_t *frame);

extern isize (*const SYSCALL_TABLE[])(const trapframe_t *frame);
extern const usize NR_SYSCALLS;
