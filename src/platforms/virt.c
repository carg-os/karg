#include <dev.h>

const dev_node_t DEV_TABLE[] = {
    {.name = "plic", .addr = 0x0C000000},
    {.name = "uart8250", .addr = 0x10000000, .irq = 10, .reg_size = 1},
    {.name = "virtio", .addr = 0x10001000},
    {.name = "virtio", .addr = 0x10002000},
    {.name = "virtio", .addr = 0x10003000},
    {.name = "virtio", .addr = 0x10004000},
    {.name = "virtio", .addr = 0x10005000},
    {.name = "virtio", .addr = 0x10006000},
    {.name = "virtio", .addr = 0x10007000},
    {.name = "virtio", .addr = 0x10008000},
    {},
};

const u32 TIMER_FREQ = 10000000;
