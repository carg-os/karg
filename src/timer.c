#include <timer.h>

#include <config.h>
#include <csr.h>
#include <errno.h>
#include <kalloc.h>

static list_node_t wait_queue = LIST_HEAD_INIT(wait_queue);

static void sched_timer_intr(time_t time) {
    register usize a0 asm("a0") = time;
    register usize a1 asm("a1");
    register usize a6 asm("a6") = 0;
    register usize a7 asm("a7") = 0x54494D45;
    asm volatile("ecall" : "+r"(a0), "=r"(a1) : "r"(a6), "r"(a7));
}

void init_timer(void) {
    csr_set_bits(sie, CSR_SIE_STIE);
    sched_timer_intr(TIME_MAX);
}

void timer_init(timer_t *timer) { list_init_head(&timer->node); }

void timer_deinit(timer_t *timer) { timer_cancel(timer); }

void timer_wait(timer_t *timer, time_t ns, void (*callback)(void *data),
                void *data) {
    timer->time = time() + ns / (NS_IN_SEC / TIMER_FREQ);
    timer->callback = callback;
    timer->data = data;

    timer_t *entry;
    list_for_each_entry(&wait_queue, node, entry) {
        if (timer->time < entry->time) {
            list_insert(&entry->node, &timer->node);
            if (list_is_front(&wait_queue, &timer->node))
                sched_timer_intr(timer->time);
            return;
        }
    }

    list_push_back(&wait_queue, &timer->node);
    if (list_is_front(&wait_queue, &timer->node))
        sched_timer_intr(timer->time);
}

void timer_cancel(timer_t *timer) {
    if (list_empty(&timer->node))
        return;
    list_remove_init(&timer->node);

    time_t wait_time = TIME_MAX;
    if (!list_empty(&wait_queue))
        wait_time = list_first_entry(&wait_queue, timer_t, node)->time;
    sched_timer_intr(wait_time);
}

void timer_handle_intr(void) {
    timer_t *timer = list_first_entry(&wait_queue, timer_t, node);
    list_remove_init(&timer->node);

    time_t wait_time = TIME_MAX;
    if (!list_empty(&wait_queue))
        wait_time = list_first_entry(&wait_queue, timer_t, node)->time;
    sched_timer_intr(wait_time);

    timer->callback(timer->data);
}
