#pragma once

#include <dev.h>
#include <types.h>

#define FB_GET_SCREEN_INFO 0
#define FB_WRITE_PIX 1
#define FB_FLUSH 2

i32 fb_register_dev(dev_t dev);
i32 fb_register_dev_probe(void (*probe)(dev_t dev));