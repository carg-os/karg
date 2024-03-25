#ifndef SYSCALL_H_
#define SYSCALL_H_

#include <trap.h>
#include <types.h>

extern isize (*const SYSCALL_TABLE[])(const trapframe_t *frame);
extern const usize NR_SYSCALLS;

#endif // SYSCALL_H_
