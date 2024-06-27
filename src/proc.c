#include <proc.h>

#include <arch/riscv/csr.h>
#include <drivers/tty.h>
#include <errno.h>
#include <mm/page_alloc.h>

void ctx_sw(usize **old_sp, usize **new_sp);
void proc_fill_stack(proc_t *proc, const proc_config_t *config, usize *usp);

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

static i32 new_fd(proc_t *proc) {
    for (i32 fd = 0; fd < PROC_FD_CAPACITY; fd++) {
        if (!(proc->fds[fd].flags & FD_FLAG_USED)) {
            proc->fds[fd].flags = FD_FLAG_USED;
            return fd;
        }
    }
    return -EAGAIN;
}

static i32 init_fds(proc_t *proc) {
    for (usize i = 0; i < PROC_FD_CAPACITY; i++) {
        proc->fds[i].flags = 0;
    }

    i32 stdin_fd = new_fd(proc);
    if (stdin_fd < 0)
        return stdin_fd;
    proc->fds[stdin_fd].dev = make_dev(TTY_DRIVER, 0);
    proc->fds[stdin_fd].flags |= FD_FLAG_READABLE;

    i32 stdout_fd = new_fd(proc);
    if (stdin_fd < 0)
        return stdin_fd;
    proc->fds[stdout_fd].dev = make_dev(TTY_DRIVER, 0);
    proc->fds[stdout_fd].flags |= FD_FLAG_WRITABLE;

    i32 stderr_fd = new_fd(proc);
    if (stdin_fd < 0)
        return stdin_fd;
    proc->fds[stderr_fd].dev = make_dev(TTY_DRIVER, 0);
    proc->fds[stderr_fd].flags |= FD_FLAG_WRITABLE;

    return 0;
}

static i32 init_stacks(proc_t *proc, const proc_config_t *config) {
    proc->kern_stack = (usize *) page_alloc();
    proc->user_stack = (usize *) page_alloc();
    if (!proc->kern_stack || !proc->user_stack)
        return -ENOMEM;

    usize *usp = (usize *) ((usize) proc->user_stack + PAGE_SIZE);
    *--usp = 0;
    for (i32 i = 0; i < config->argc; i++) {
        *--usp = (usize) config->argv[config->argc - 1 - i];
    }

    proc->sp = (usize *) ((usize) proc->kern_stack + PAGE_SIZE);
    proc_fill_stack(proc, config, usp);

    return 0;
}

i32 proc_init(proc_t *proc, const proc_config_t *config) {
    proc->pid = new_pid();
    if (proc->pid < 0)
        return proc->pid;
    proc->flags = config->flags;

    proc->parent = config->parent;
    list_init_head(&proc->children);
    list_init_head(&proc->zombie_children);

    i32 res = init_fds(proc);
    if (res < 0)
        return res;

    proc->state = PROC_STATE_INIT;
    timer_init(&proc->timer);

    res = init_stacks(proc, config);
    if (res < 0)
        return res;

    proc_table[proc->pid] = proc;
    if (config->parent)
        list_push_back(&config->parent->children, &proc->tree_node);
    return 0;
}

void proc_deinit(proc_t *proc) {
    list_remove(&proc->tree_node);
    proc_table[proc->pid] = nullptr;
    page_free(proc->user_stack);
    page_free(proc->kern_stack);
}

void proc_ctx_sw(proc_t *old_proc, proc_t *new_proc) {
    usize *null = nullptr;
    usize **old_sp = old_proc ? &old_proc->sp : &null;
    usize **new_sp = &new_proc->sp;
    ctx_sw(old_sp, new_sp);
}

void proc_adopt(proc_t *new_parent, proc_t *proc) {
    list_remove(&proc->tree_node);
    proc->parent = new_parent;
    if (proc->state != PROC_STATE_ZOMBIE) {
        list_push_back(&new_parent->children, &proc->tree_node);
    } else {
        list_push_back(&new_parent->zombie_children, &proc->tree_node);
    }
}

bool proc_is_bad_fd(const proc_t *proc, i32 fd) {
    return fd < 0 || fd >= PROC_FD_CAPACITY ||
           !(proc->fds[fd].flags & FD_FLAG_USED);
}
