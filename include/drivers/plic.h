#pragma once

#include <types.h>

i32 plic_enable_irq(u32 irq);
i32 plic_claim(u32 *irq);
i32 plic_complete(u32 irq);
