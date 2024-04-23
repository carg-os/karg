#include <proc.h>

#include <config.h>
#include <csr.h>
#include <errno.h>
#include <kalloc.h>
#include <page_alloc.h>

void ctx_sw(usize **old_sp, usize **new_sp);
void proc_entry(void);

proc_t *proc_table[PROC_TABLE_SIZE];

static pid_t new_pid(void) {
    static pid_t last_pid = PROC_TABLE_SIZE - 1;
    for (pid_t pid = last_pid + 1; pid < PROC_TABLE_SIZE; pid++) {
        if (!proc_table[pid]) {
            last_pid = pid;
            return pid;
        }
    }
    for (pid_t pid = 0; pid < last_pid; pid++) {
        if (!proc_table[pid]) {
            last_pid = pid;
            return pid;
        }
    }
    return -EAGAIN;
}

i32 proc_init(proc_t *proc, void *entry, u32 flags, proc_t *parent, i32 argc,
              char *argv[]) {
    proc->pid = new_pid();
    if (proc->pid < 0)
        return proc->pid;
    proc->flags = flags;
    proc->parent = parent;
    list_init_head(&proc->children);
    list_init_head(&proc->zombie_children);

    proc->state = PROC_STATE_INIT;
    timer_init(&proc->timer);

    proc->kstack = (usize *) page_alloc();
    proc->ustack = (usize *) page_alloc();
    if (!proc->kstack || !proc->ustack)
        return -ENOMEM;

    usize *usp = (usize *) ((u8 *) proc->ustack + PAGE_SIZE);
    for (i32 i = 0; i < argc; i++) {
        *--usp = (usize) argv[argc - 1 - i];
    }

    proc->sp = (usize *) ((u8 *) proc->kstack + PAGE_SIZE);
    proc->sp -= 22;
    proc->sp[21] = (usize) proc;
    proc->sp[20] = (usize) entry;
    proc->sp[19] = (usize) usp;
    proc->sp[18] = argc;
    proc->sp[17] = (usize) usp;
    proc->sp[15] = (usize) proc_entry;

    proc_table[proc->pid] = proc;
    if (parent)
        list_push_back(&parent->children, &proc->tree_node);
    return 0;
}

void proc_deinit(proc_t *proc) {
    list_remove(&proc->tree_node);
    proc_table[proc->pid] = nullptr;
    page_free(proc->ustack);
    page_free(proc->kstack);
    timer_deinit(&proc->timer);
}

void proc_setup(proc_t *proc) {
    csr_set_bits(sstatus, CSR_SSTATUS_SPIE);
    if (proc->flags & PROC_FLAG_KERNEL) {
        csr_set_bits(sstatus, CSR_SSTATUS_SPP);
    } else {
        csr_clear_bits(sstatus, CSR_SSTATUS_SPP);
    }
}

void proc_ctx_sw(proc_t *old_proc, proc_t *new_proc) {
    usize *null = nullptr;
    usize **old_sp = old_proc ? &old_proc->sp : &null;
    usize **new_sp = &new_proc->sp;
    ctx_sw(old_sp, new_sp);
}

void proc_adopt(proc_t *parent, proc_t *proc) {
    list_remove(&proc->tree_node);
    proc->parent = parent;
    if (proc->state != PROC_STATE_ZOMBIE) {
        list_push_back(&parent->children, &proc->tree_node);
    } else {
        list_push_back(&parent->zombie_children, &proc->tree_node);
    }
}
