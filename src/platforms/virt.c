#include <dev.h>

#include <dev_table.h>

dev_node_t dev_table[] = {
    {.name = "uart", .addr = 0x10000000, .irq = 10, .reg_size = 1},
    {},
};
