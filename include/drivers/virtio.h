#pragma once

#include <config.h>
#include <dev.h>
#include <types.h>

#define VIRTIO_DESC_FLAG_NEXT 0x1
#define VIRTIO_DESC_FLAG_WRITE 0x2
#define VIRTIO_DESC_FLAG_INDIRECT 0x4

typedef struct {
    u64 addr;
    u32 len;
    u16 flags;
    u16 next;
} virtio_desc_t;

typedef struct {
    u16 flags;
    u16 idx;
    u16 ring[VIRTIO_RING_SIZE];
} virtio_avail_queue_t;

typedef struct {
    u32 id;
    u32 len;
} virtio_used_elem_t;

typedef struct {
    u16 flags;
    u16 idx;
    virtio_used_elem_t ring[VIRTIO_RING_SIZE];
} virtio_used_queue_t;

#define FEATURES 0x20
#define QUEUE_SEL 0x30
#define QUEUE_NUM 0x38
#define QUEUE_READY 0x44
#define NOTIFY 0x50
#define STATUS 0x70
#define QUEUE_DESC_LOW 0x80
#define QUEUE_DRIVER_LOW 0x90
#define QUEUE_DEVICE_LOW 0xA0

i32 init_virtio_gpu(const dev_node_t *node);
