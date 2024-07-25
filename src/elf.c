#include <errno.h>
#include <init.h>
#include <math.h>
#include <mm/page_alloc.h>
#include <mm/vm.h>
#include <module/init.h>
#include <module/log.h>
#include <module/module.h>
#include <utils/mem.h>

MODULE_NAME("elf");

enum {
    EI_MAG0,
    EI_MAG1,
    EI_MAG2,
    EI_MAG3,
    EI_CLASS,
    EI_DATA,
    EI_VERSION,
};

typedef struct {
    u8 ident[16];
    u16 type;
    u16 machine;
    u32 version;
    usize entry;
    usize phoff;
    usize shoff;
    u32 flags;
    u16 ehsize;
    u16 phentsize;
    u16 phnum;
    u16 shentsize;
    u16 shnum;
    u16 shstrndx;
} elf_hdr_t;

enum {
    PT_LOAD = 1,
};

#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

typedef struct {
    u32 type;
    u32 flags;
    usize offset;
    usize vaddr;
    usize paddr;
    usize filesz;
    usize memsz;
    usize align;
} prog_hdr_t;

void *init_elf_entry;
void *init_page_table;

static i32 check_ident(const u8 *ident) {
    if (ident[EI_MAG0] != 0x7F)
        return false;
    if (ident[EI_MAG1] != 'E')
        return false;
    if (ident[EI_MAG2] != 'L')
        return false;
    if (ident[EI_MAG3] != 'F')
        return false;
    return true;
}

static i32 check_compat(const u8 *ident) {
    if (ident[EI_CLASS] != 2)
        return false;
    if (ident[EI_DATA] != 1)
        return false;
    if (ident[EI_VERSION] != 1)
        return false;
    return true;
}

static i32 init(void) {
    const elf_hdr_t *elf_hdr = (const elf_hdr_t *) INIT_ELF;
    init_elf_entry = (void *) elf_hdr->entry;

    if (!check_ident(elf_hdr->ident))
        return -EACCES;

    if (!check_compat(elf_hdr->ident) || elf_hdr->type != 2)
        return -EINVAL;

    init_page_table = vm_create_page_table();
    if (!init_page_table)
        return -ENOMEM;

    const prog_hdr_t *prog_hdrs =
        (const prog_hdr_t *) (INIT_ELF + elf_hdr->phoff);
    for (u16 i = 0; i < elf_hdr->phnum; i++) {
        const prog_hdr_t *prog_hdr = prog_hdrs + i;
        if (prog_hdr->type != PT_LOAD)
            continue;

        u32 flags = VM_FLAG_USER;
        if (prog_hdr->flags & PF_X)
            flags |= VM_FLAG_EXECUTABLE;
        if (prog_hdr->flags & PF_W)
            flags |= VM_FLAG_WRITABLE;
        if (prog_hdr->flags & PF_R)
            flags |= VM_FLAG_READABLE;

        for (usize offset = 0; offset < prog_hdr->memsz; offset += PAGE_SIZE) {
            void *page = page_alloc();
            if (!page)
                return -ENOMEM;
            mem_set(page, 0, PAGE_SIZE);
            if (offset < prog_hdr->filesz) {
                const void *src = INIT_ELF + prog_hdr->offset + offset;
                mem_copy(page, src, min(prog_hdr->filesz - offset, PAGE_SIZE));
            }
            i32 res = vm_map_page(init_page_table, prog_hdr->vaddr + offset,
                                  page, flags);
            if (res < 0)
                return res;
        }
    }

    return 0;
}

module_post_init(init);
