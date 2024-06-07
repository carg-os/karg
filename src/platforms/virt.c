#include <dev_table.h>

dev_node_t dev_table[] = {
    {.name = "plic", .addr = 0x0C000000},
    {.name = "uart", .addr = 0x10000000, .irq = 10, .reg_size = 1},
    {},
};
