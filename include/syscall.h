#pragma once

#include <trap.h>

extern isize (*const SYSCALL_TABLE[])(const trapframe_t *frame);
extern const usize NR_SYSCALLS;
