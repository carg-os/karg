#pragma once

#include <proc.h>

typedef struct {
    void *entry;
    void *page_table;
} elf_load_res_t;

i32 elf_load(elf_load_res_t *load_res);
