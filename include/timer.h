#pragma once

#include <list.h>
#include <time.h>

typedef struct {
    list_node_t node;

    time_t time;
    void (*callback)(void *data);
    void *data;
} timer_t;

void timer_init(timer_t *timer);
void timer_wait(timer_t *timer, time_t ns, void (*callback)(void *data),
                void *data);
void timer_cancel(timer_t *timer);
void timer_isr(void);
