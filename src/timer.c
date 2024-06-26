#include <timer.h>

#include <arch/riscv/csr.h>
#include <arch/riscv/sbi.h>
#include <module/init.h>
#include <module/module.h>

extern const u32 TIMER_FREQ;

MODULE_NAME("timer");

static list_node_t wait_queue = list_head_init(wait_queue);

static i32 init(void) {
    csr_set_bits(sie, CSR_SIE_STIE);
    return sbi_set_timer(TIME_MAX);
}

module_pre_init(init);

void timer_init(timer_t *timer) { list_init_head(&timer->node); }

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
                sbi_set_timer(timer->time);
            return;
        }
    }

    list_push_back(&wait_queue, &timer->node);
    if (list_is_front(&wait_queue, &timer->node))
        sbi_set_timer(timer->time);
}

void timer_cancel(timer_t *timer) {
    if (list_empty(&timer->node))
        return;
    list_remove_init(&timer->node);

    time_t wait_time = TIME_MAX;
    if (!list_empty(&wait_queue))
        wait_time = list_first_entry(&wait_queue, timer_t, node)->time;
    sbi_set_timer(wait_time);
}

void timer_isr(void) {
    timer_t *timer = list_first_entry(&wait_queue, timer_t, node);
    list_remove_init(&timer->node);

    time_t wait_time = TIME_MAX;
    if (!list_empty(&wait_queue))
        wait_time = list_first_entry(&wait_queue, timer_t, node)->time;
    sbi_set_timer(wait_time);

    timer->callback(timer->data);
}
