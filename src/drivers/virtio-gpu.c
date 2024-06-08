#include <dev.h>
#include <drivers/fb.h>
#include <init.h>
#include <types.h>

enum virtio_gpu_ctrl_type {
    /* 2d commands */
    VIRTIO_GPU_CMD_GET_DISPLAY_INFO = 0x0100,
    VIRTIO_GPU_CMD_RESOURCE_CREATE_2D,
    VIRTIO_GPU_CMD_RESOURCE_UNREF,
    VIRTIO_GPU_CMD_SET_SCANOUT,
    VIRTIO_GPU_CMD_RESOURCE_FLUSH,
    VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D,
    VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING,
    VIRTIO_GPU_CMD_RESOURCE_DETACH_BACKING,
    VIRTIO_GPU_CMD_GET_CAPSET_INFO,
    VIRTIO_GPU_CMD_GET_CAPSET,
    VIRTIO_GPU_CMD_GET_EDID,

    /* cursor commands */
    VIRTIO_GPU_CMD_UPDATE_CURSOR = 0x0300,
    VIRTIO_GPU_CMD_MOVE_CURSOR,

    /* success responses */
    VIRTIO_GPU_RESP_OK_NODATA = 0x1100,
    VIRTIO_GPU_RESP_OK_DISPLAY_INFO,
    VIRTIO_GPU_RESP_OK_CAPSET_INFO,
    VIRTIO_GPU_RESP_OK_CAPSET,
    VIRTIO_GPU_RESP_OK_EDID,

    /* error responses */
    VIRTIO_GPU_RESP_ERR_UNSPEC = 0x1200,
    VIRTIO_GPU_RESP_ERR_OUT_OF_MEMORY,
    VIRTIO_GPU_RESP_ERR_INVALID_SCANOUT_ID,
    VIRTIO_GPU_RESP_ERR_INVALID_RESOURCE_ID,
    VIRTIO_GPU_RESP_ERR_INVALID_CONTEXT_ID,
    VIRTIO_GPU_RESP_ERR_INVALID_PARAMETER,
};

#define VIRTIO_GPU_FLAG_FENCE (1 << 0)

struct virtio_gpu_ctrl_hdr {
    u32 type;
    u32 flags;
    u64 fence_id;
    u32 ctx_id;
    u32 padding;
};

enum virtio_gpu_formats {
    VIRTIO_GPU_FORMAT_B8G8R8A8_UNORM = 1,
    VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM = 2,
    VIRTIO_GPU_FORMAT_A8R8G8B8_UNORM = 3,
    VIRTIO_GPU_FORMAT_X8R8G8B8_UNORM = 4,

    VIRTIO_GPU_FORMAT_R8G8B8A8_UNORM = 67,
    VIRTIO_GPU_FORMAT_X8B8G8R8_UNORM = 68,

    VIRTIO_GPU_FORMAT_A8B8G8R8_UNORM = 121,
    VIRTIO_GPU_FORMAT_R8G8B8X8_UNORM = 134,
};

struct virtio_gpu_resource_create_2d {
    struct virtio_gpu_ctrl_hdr hdr;
    u32 resource_id;
    u32 format;
    u32 width;
    u32 height;
};

struct virtio_gpu_resource_attach_backing {
    struct virtio_gpu_ctrl_hdr hdr;
    u32 resource_id;
    u32 nr_entries;
};

struct virtio_gpu_mem_entry {
    u64 addr;
    u32 length;
    u32 padding;
};

struct virtio_gpu_rect {
    u32 x;
    u32 y;
    u32 width;
    u32 height;
};

struct virtio_gpu_set_scanout {
    struct virtio_gpu_ctrl_hdr hdr;
    struct virtio_gpu_rect r;
    u32 scanout_id;
    u32 resource_id;
};

struct virtio_gpu_resource_flush {
    struct virtio_gpu_ctrl_hdr hdr;
    struct virtio_gpu_rect r;
    u32 resource_id;
    u32 padding;
};

struct virtio_gpu_transfer_to_host_2d {
    struct virtio_gpu_ctrl_hdr hdr;
    struct virtio_gpu_rect r;
    u64 offset;
    u32 resource_id;
    u32 padding;
};

#define VIRTQ_DESC_F_NEXT 1
#define VIRTQ_DESC_F_WRITE 2
#define VIRTQ_DESC_F_INDIRECT 4

struct virtq_desc {
    u64 addr;
    u32 len;
    u16 flags;
    u16 next;
};

struct virtq_avail {
    u16 flags;
    u16 idx;
    u16 ring[128];
};

struct virtq_used_elem {
    u32 id;
    u32 len;
};

struct virtq_used {
    u16 flags;
    u16 idx;
    struct virtq_used_elem ring[128];
};

#define REG(reg) *((volatile u32 *) (0x10008000 + (reg)))
#define FEATURES 0x20
#define QUEUE_SEL 0x30
#define QUEUE_NUM 0x38
#define QUEUE_READY 0x44
#define NOTIFY 0x50
#define STATUS 0x70
#define QUEUE_DESC_LOW 0x80
#define QUEUE_DRIVER_LOW 0x90
#define QUEUE_DEVICE_LOW 0xA0

#define STATUS_ACK 1
#define STATUS_DRIVER 2
#define STATUS_DRIVER_OK 4
#define STATUS_FEATURES_OK 8

static struct virtq_desc desc[128];
static struct virtq_avail avail;
static struct virtq_used used;

static struct virtio_gpu_resource_create_2d create_2d;
static struct virtio_gpu_resource_attach_backing attach_backing;
static struct virtio_gpu_mem_entry gpu_mem_entry;
static struct virtio_gpu_set_scanout set_scanout;
static struct virtio_gpu_transfer_to_host_2d transfer;
static struct virtio_gpu_resource_flush res_flush;
static struct virtio_gpu_ctrl_hdr hdr[5];

#define FB_WIDTH 1920
#define FB_HEIGHT 1080
#define FB_PITCH 1920 * 4

static u32 fb[FB_WIDTH * FB_HEIGHT];
static usize offset = 0;

static void flush(u32 y, u32 height) {
    while (avail.idx != used.idx)
        ;
    transfer.r.x = 0;
    transfer.r.y = y;
    transfer.r.width = FB_WIDTH;
    transfer.r.height = height;
    transfer.offset = offset + y * FB_PITCH;

    res_flush.r.x = 0;
    res_flush.r.y = y;
    res_flush.r.width = FB_WIDTH;
    res_flush.r.height = height;

    desc[0].addr = (usize) &transfer;
    desc[0].len = sizeof(transfer);
    desc[0].flags = VIRTQ_DESC_F_NEXT;
    desc[0].next = 1;

    desc[1].addr = (usize) &hdr[0];
    desc[1].len = sizeof(hdr[0]);
    desc[1].flags = VIRTQ_DESC_F_WRITE;
    desc[1].next = 0;

    desc[2].addr = (usize) &res_flush;
    desc[2].len = sizeof(res_flush);
    desc[2].flags = VIRTQ_DESC_F_NEXT;
    desc[2].next = 3;

    desc[3].addr = (usize) &hdr[1];
    desc[3].len = sizeof(hdr[1]);
    desc[3].flags = VIRTQ_DESC_F_WRITE;
    desc[3].next = 0;

    avail.ring[avail.idx++ % 128] = 0;
    avail.ring[avail.idx++ % 128] = 2;
    REG(NOTIFY) = 0;
}

static i32 ioctl(u32 num, u32 req, va_list args) {
    (void) num;
    switch (req) {
    case 0: {
        u32 *width = va_arg(args, u32 *);
        u32 *height = va_arg(args, u32 *);
        *width = FB_WIDTH;
        *height = FB_HEIGHT;
        break;
    }
    case 1: {
        u32 x = va_arg(args, u32);
        u32 y = va_arg(args, u32);
        u32 val = va_arg(args, u32);
        fb[y * FB_WIDTH + x] = val;
        break;
    }
    case 2: {
        u32 y = va_arg(args, u32);
        u32 height = va_arg(args, u32);
        flush(y, height);
        break;
    }
    }
    return 0;
}

static driver_t driver = {
    .nr_devs = 1,
    .read = nullptr,
    .write = nullptr,
    .ioctl = ioctl,
};

static i32 init(void) {
    REG(STATUS) = 0;
    REG(STATUS) |= STATUS_ACK;
    REG(STATUS) |= STATUS_DRIVER;
    REG(FEATURES) = 0;
    REG(STATUS) |= STATUS_FEATURES_OK;

    REG(QUEUE_SEL) = 0;
    REG(QUEUE_NUM) = 128;
    REG(QUEUE_DESC_LOW) = (usize) desc;
    REG(QUEUE_DRIVER_LOW) = (usize) &avail;
    REG(QUEUE_DEVICE_LOW) = (usize) &used;

    REG(QUEUE_READY) = 1;

    REG(STATUS) |= STATUS_DRIVER_OK;

    create_2d.hdr.type = VIRTIO_GPU_CMD_RESOURCE_CREATE_2D;
    create_2d.hdr.flags = 0;
    create_2d.hdr.fence_id = 0;
    create_2d.hdr.ctx_id = 0;
    create_2d.resource_id = 1;
    create_2d.format = VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM;
    create_2d.width = FB_WIDTH;
    create_2d.height = FB_HEIGHT;

    attach_backing.hdr.type = VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING;
    attach_backing.hdr.flags = 0;
    attach_backing.hdr.fence_id = 0;
    attach_backing.hdr.ctx_id = 0;
    attach_backing.resource_id = 1;
    attach_backing.nr_entries = 1;

    gpu_mem_entry.addr = (usize) fb;
    gpu_mem_entry.length = sizeof(fb);

    set_scanout.hdr.type = VIRTIO_GPU_CMD_SET_SCANOUT;
    set_scanout.hdr.flags = 0;
    set_scanout.hdr.fence_id = 0;
    set_scanout.hdr.ctx_id = 0;
    set_scanout.r.x = 0;
    set_scanout.r.y = 0;
    set_scanout.r.width = FB_WIDTH;
    set_scanout.r.height = FB_HEIGHT;
    set_scanout.scanout_id = 0;
    set_scanout.resource_id = 1;

    transfer.hdr.type = VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D;
    transfer.hdr.flags = 0;
    transfer.hdr.fence_id = 0;
    transfer.hdr.ctx_id = 0;
    transfer.r.x = 0;
    transfer.r.y = 0;
    transfer.r.width = FB_WIDTH;
    transfer.r.height = FB_HEIGHT;
    transfer.offset = 0;
    transfer.resource_id = 1;

    res_flush.hdr.type = VIRTIO_GPU_CMD_RESOURCE_FLUSH;
    res_flush.hdr.flags = 0;
    res_flush.hdr.fence_id = 0;
    res_flush.hdr.ctx_id = 0;
    res_flush.r.x = 0;
    res_flush.r.y = 0;
    res_flush.r.width = FB_WIDTH;
    res_flush.r.height = FB_HEIGHT;
    res_flush.resource_id = 1;

    desc[0].addr = (usize) &create_2d;
    desc[0].len = sizeof(create_2d);
    desc[0].flags = VIRTQ_DESC_F_NEXT;
    desc[0].next = 1;

    desc[1].addr = (usize) &hdr[0];
    desc[1].len = sizeof(hdr[0]);
    desc[1].flags = VIRTQ_DESC_F_WRITE;
    desc[1].next = 0;

    desc[2].addr = (usize) &attach_backing;
    desc[2].len = sizeof(attach_backing);
    desc[2].flags = VIRTQ_DESC_F_NEXT;
    desc[2].next = 3;

    desc[3].addr = (usize) &gpu_mem_entry;
    desc[3].len = sizeof(gpu_mem_entry);
    desc[3].flags = VIRTQ_DESC_F_NEXT;
    desc[3].next = 4;

    desc[4].addr = (usize) &hdr[1];
    desc[4].len = sizeof(hdr[1]);
    desc[4].flags = VIRTQ_DESC_F_WRITE;
    desc[4].next = 0;

    desc[5].addr = (usize) &set_scanout;
    desc[5].len = sizeof(set_scanout);
    desc[5].flags = VIRTQ_DESC_F_NEXT;
    desc[5].next = 6;

    desc[6].addr = (usize) &hdr[2];
    desc[6].len = sizeof(hdr[2]);
    desc[6].flags = VIRTQ_DESC_F_WRITE;
    desc[6].next = 0;

    desc[7].addr = (usize) &transfer;
    desc[7].len = sizeof(transfer);
    desc[7].flags = VIRTQ_DESC_F_NEXT;
    desc[7].next = 8;

    desc[8].addr = (usize) &hdr[3];
    desc[8].len = sizeof(hdr[3]);
    desc[8].flags = VIRTQ_DESC_F_WRITE;
    desc[8].next = 0;

    desc[9].addr = (usize) &res_flush;
    desc[9].len = sizeof(res_flush);
    desc[9].flags = VIRTQ_DESC_F_NEXT;
    desc[9].next = 10;

    desc[10].addr = (usize) &hdr[4];
    desc[10].len = sizeof(hdr[4]);
    desc[10].flags = VIRTQ_DESC_F_WRITE;
    desc[10].next = 0;

    avail.ring[avail.idx++] = 0;
    avail.ring[avail.idx++] = 2;
    avail.ring[avail.idx++] = 5;
    avail.ring[avail.idx++] = 7;
    avail.ring[avail.idx++] = 9;
    REG(NOTIFY) = 0;

    dev_t dev = {
        .driver = &driver,
        .num = 0,
    };
    fb_register_dev(dev);

    return 0;
}

MODULE_POST_INIT(init);
