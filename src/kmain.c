#include <arch/riscv/csr.h>
#include <elf.h>
#include <errno.h>
#include <mm/page_alloc.h>
#include <mm/vm.h>
#include <module/init.h>
#include <module/log.h>
#include <module/module.h>
#include <sched.h>

MODULE_NAME("kmain");

[[noreturn]] void kmain(void) {
    init_modules();

    elf_load_res_t elf_load_res;
    i32 res = elf_load(&elf_load_res);
    if (res < 0)
        log_panic(
            "errno %s is returned when parsing the ELF file of init process",
            errno_name(-res));

    proc_t init_proc;
    res = proc_init(&init_proc, elf_load_res.entry, 0, nullptr, 0, nullptr,
                    elf_load_res.page_table);
    if (res < 0)
        log_panic("errno %s is returned when initializing init process",
                  errno_name(-res));

    sched_update_state(&init_proc, PROC_STATE_READY);
    sched_start();
}
