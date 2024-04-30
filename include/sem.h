#pragma once

#include <list.h>
#include <types.h>

typedef struct {
    list_node_t wait_queue;
    i32 val;
} sem_t;

void sem_init(sem_t *sem);
void sem_deinit(sem_t *sem);
void sem_wait(sem_t *sem);
void sem_signal(sem_t *sem);
void sem_signaln(sem_t *sem, usize n);
