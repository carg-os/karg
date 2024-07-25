#pragma once

#include <types.h>

#define VM_FLAG_READABLE 0x1
#define VM_FLAG_WRITABLE 0x2
#define VM_FLAG_EXECUTABLE 0x4
#define VM_FLAG_USER 0x8

void *vm_create_page_table(void);
i32 vm_map_page(void *page_table, usize virt, void *phys, u32 flags);
i32 vm_virt_to_phys(void *page_table, usize virt, usize *phys);
