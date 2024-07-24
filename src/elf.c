#include <errno.h>
#include <init.h>
#include <module/init.h>
#include <module/log.h>
#include <module/module.h>
#include <utils/mem.h>

MODULE_NAME("elf");

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

enum {
    EI_MAG0,
    EI_MAG1,
    EI_MAG2,
    EI_MAG3,
    EI_CLASS,
    EI_DATA,
    EI_VERSION,
};

enum {
    PT_LOAD = 0x00000001,
};

void *elf_entry;

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
    elf_entry = (void *) elf_hdr->entry;

    if (!check_ident(elf_hdr->ident))
        return -EACCES;

    if (!check_compat(elf_hdr->ident) || elf_hdr->type != 2)
        return -EINVAL;

    const prog_hdr_t *prog_hdrs =
        (const prog_hdr_t *) (INIT_ELF + elf_hdr->phoff);
    for (u16 i = 0; i < elf_hdr->phnum; i++) {
        const prog_hdr_t *prog_hdr = prog_hdrs + i;
        if (prog_hdr->type != PT_LOAD)
            continue;

        const void *src = INIT_ELF + prog_hdr->offset;
        void *dst = (void *) prog_hdr->paddr;
        mem_set(dst, 0x00, prog_hdr->memsz);
        mem_copy(dst, src, prog_hdr->filesz);
    }

    return 0;
}

module_post_init(init);
