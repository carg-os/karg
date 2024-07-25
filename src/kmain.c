#include <arch/riscv/csr.h>
#include <elf.h>
#include <errno.h>
#include <mm/page_alloc.h>
#include <mm/vm.h>
#include <module/init.h>
#include <module/log.h>
#include <module/module.h>
#include <sched.h>

void trampoline(void);

MODULE_NAME("kmain");

static proc_t init_proc;

[[noreturn]] void kmain(void) {
    init_modules();
    i32 res = proc_init(&init_proc, init_elf_entry, 0, nullptr, 0, nullptr);
    if (res < 0)
        log_panic("errno %s is returned when initializing init process",
                  errno_name(-res));
    vm_map_page(init_page_table, (usize) trampoline, trampoline,
                VM_FLAG_READABLE | VM_FLAG_EXECUTABLE);
    vm_map_page(init_page_table, (usize) init_proc.kern_stack,
                init_proc.kern_stack, VM_FLAG_READABLE | VM_FLAG_WRITABLE);
    vm_map_page(init_page_table, (usize) init_proc.user_stack,
                init_proc.user_stack,
                VM_FLAG_READABLE | VM_FLAG_WRITABLE | VM_FLAG_USER);
    sched_update_state(&init_proc, PROC_STATE_READY);
    sched_start();
}
