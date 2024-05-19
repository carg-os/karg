#include <platform.h>

#include <timer.h>

static timer_t watchdog_timer;

static void watchdog_reset(void *data) {
    (void) data;
    *((volatile u32 *) 0x06011010) = 0x000014AF;
    timer_wait(&watchdog_timer, 5 * NS_IN_SEC, watchdog_reset, nullptr);
}

static i32 init_watchdog(void) {
    *((volatile u32 *) 0x06011014) = 0x16AA0001;
    *((volatile u32 *) 0x06011018) = 0x16AA0081;

    timer_init(&watchdog_timer);
    timer_wait(&watchdog_timer, 5 * NS_IN_SEC, watchdog_reset, nullptr);

    return 0;
}

i32 init_platform(void) {
    i32 res = init_watchdog();
    if (res < 0)
        return res;

    return 0;
}
