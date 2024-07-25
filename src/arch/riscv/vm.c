#include <mm/vm.h>

#include <errno.h>
#include <mm/page_alloc.h>
#include <utils/mem.h>

typedef struct {
    u8 flags;
    u8 reserved : 2;
    u64 ppn : 44;
    u16 padding : 10;
} pte_t;

#define PTE_FLAG_V 0x01
#define PTE_FLAG_R 0x02
#define PTE_FLAG_W 0x04
#define PTE_FLAG_X 0x08
#define PTE_FLAG_U 0x10
#define PTE_FLAG_G 0x20
#define PTE_FLAG_A 0x40
#define PTE_FLAG_D 0x80

static void init_pte(pte_t *pte, void *page, u32 flags) {
    pte->flags |= PTE_FLAG_V;

    if (flags & VM_FLAG_READABLE)
        pte->flags |= PTE_FLAG_R;
    if (flags & VM_FLAG_WRITABLE)
        pte->flags |= PTE_FLAG_W;
    if (flags & VM_FLAG_EXECUTABLE)
        pte->flags |= PTE_FLAG_X;
    if (flags & VM_FLAG_USER)
        pte->flags |= PTE_FLAG_U;

    pte->ppn = (usize) page / PAGE_SIZE;
}

void *vm_create_page_table(void) {
    pte_t *page_table = (pte_t *) page_alloc();
    if (!page_table)
        return nullptr;
    mem_set(page_table, 0, PAGE_SIZE);
    return page_table;
}

i32 vm_map_page(void *page_table, usize virt, void *phys, u32 flags) {
    pte_t *page_table0 = (pte_t *) page_table;
    u16 idx0 = virt >> 30;
    pte_t *pte0 = page_table0 + idx0;

    pte_t *page_table1 = (pte_t *) (usize) (pte0->ppn * PAGE_SIZE);
    if (!(pte0->flags & PTE_FLAG_V)) {
        page_table1 = (pte_t *) vm_create_page_table();
        if (!page_table1)
            return -ENOMEM;
        init_pte(pte0, page_table1, 0);
    }
    u16 idx1 = (virt >> 21) & 0x1FF;
    pte_t *pte1 = page_table1 + idx1;

    pte_t *page_table2 = (pte_t *) (usize) (pte1->ppn * PAGE_SIZE);
    if (!(pte1->flags & PTE_FLAG_V)) {
        page_table2 = (pte_t *) vm_create_page_table();
        if (!page_table2)
            return -ENOMEM;
        init_pte(pte1, page_table2, 0);
    }
    u16 idx2 = (virt >> 12) & 0x1FF;
    pte_t *pte2 = page_table2 + idx2;

    init_pte(pte2, phys, flags);
    return 0;
}

i32 vm_virt_to_phys(void *page_table, usize virt, usize *phys) {
    if (virt >> 39)
        return -EFAULT;

    pte_t *page_table0 = (pte_t *) page_table;
    u16 idx0 = virt >> 30;
    pte_t *pte0 = page_table0 + idx0;

    pte_t *page_table1 = (pte_t *) (usize) (pte0->ppn * PAGE_SIZE);
    if (!(pte0->flags & PTE_FLAG_V))
        return -EFAULT;
    u16 idx1 = (virt >> 21) & 0x1FF;
    pte_t *pte1 = page_table1 + idx1;

    pte_t *page_table2 = (pte_t *) (usize) (pte1->ppn * PAGE_SIZE);
    if (!(pte1->flags & PTE_FLAG_V))
        return -EFAULT;
    u16 idx2 = (virt >> 12) & 0x1FF;
    pte_t *pte2 = page_table2 + idx2;

    *phys = (pte2->ppn << 12) + virt % PAGE_SIZE;
    return 0;
}
