#include <dev_table.h>

const dev_node_t DEV_TABLE[] = {
    {.name = "plic", .addr = 0x0C000000},
    {.name = "uart", .addr = 0x10000000, .irq = 10, .reg_size = 1},
    {},
};

const u32 TIMER_FREQ = 10000000;
