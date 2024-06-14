#include <drivers/virtio.h>

#include <dev.h>
#include <log.h>
#include <module.h>

MODULE_NAME("virtio");

#define REG(base, reg) *((volatile u32 *) (base + reg))
#define DEV_ID 0x8

static i32 init_dev(const dev_node_t *node) {
    switch (REG(node->addr, DEV_ID)) {
    case 16:
        init_virtio_gpu(node);
        log_info("initialized virtio-gpu at 0x%p", node->addr);
        break;
    }
    return 0;
}

dev_init("virtio", init_dev);
