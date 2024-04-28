#include <syscall.h>

#include <errno.h>
#include <kalloc.h>
#include <sched.h>
#include <uart.h>

#define REBOOT_REBOOT 0
#define REBOOT_SHUTDOWN 1

[[noreturn]] static isize sys_exit(const trapframe_t *frame) {
    i32 status = frame->a0;
    proc_t *proc = curr_proc;

    proc->exit_status = status;

    while (!list_empty(&proc->children)) {
        proc_t *child = list_first_entry(&proc->children, proc_t, tree_node);
        proc_adopt(proc_table[1], child);
    }

    while (!list_empty(&proc->zombie_children)) {
        proc_t *child =
            list_first_entry(&proc->zombie_children, proc_t, tree_node);
        proc_adopt(proc_table[1], child);
    }

    proc_t *parent = proc->parent;
    list_remove(&proc->tree_node);
    list_push_back(&parent->zombie_children, &proc->tree_node);

    if (parent->state == PROC_STATE_WAIT_CHILD) {
        parent->proc_waiting = proc;
        sched_update_state(parent, PROC_STATE_READY);
    }

    sched_update_state(proc, PROC_STATE_ZOMBIE);
    unreachable();
}

static isize sys_read(const trapframe_t *frame) {
    isize fd = frame->a0;
    u8 *buf = (u8 *) frame->a1;
    usize size = frame->a2;
    if (fd != 0)
        return -EBADF;
    if (!buf)
        return -EFAULT;

    for (usize i = 0; i < size; i++) {
        buf[i] = uart_getc();
    }
    return size;
}

static isize sys_write(const trapframe_t *frame) {
    isize fd = frame->a0;
    const u8 *buf = (const u8 *) frame->a1;
    usize size = frame->a2;
    if (fd != 1)
        return -EBADF;
    if (!buf)
        return -EFAULT;

    for (usize i = 0; i < size; i++) {
        uart_putc(buf[i]);
    }
    return size;
}

static isize sys_yield(const trapframe_t *frame) {
    (void) frame;
    sched_resched();
    return 0;
}

static void wakeup_proc(void *data) {
    sched_update_state((proc_t *) data, PROC_STATE_READY);
}

static isize sys_sleepns(const trapframe_t *frame) {
    time_t ns = frame->a0;

    timer_wait(&curr_proc->timer, ns, wakeup_proc, curr_proc);
    sched_update_state(curr_proc, PROC_STATE_SLEEP);
    return 0;
}

static isize sys_getpid(const trapframe_t *frame) {
    (void) frame;
    return curr_proc->pid;
}

static isize sys_proc(const trapframe_t *frame) {
    void *entry = (void *) frame->a0;
    char **argv = (char **) frame->a1;
    if (!entry || !argv)
        return -EFAULT;
    usize argc;
    for (argc = 0; argv[argc]; argc++)
        ;

    proc_t *proc = (proc_t *) kmalloc(sizeof(proc_t));
    if (!proc)
        return -ENOMEM;
    i32 res = proc_init(proc, entry, 0, curr_proc, argc, argv);
    if (res < 0)
        return res;

    sched_update_state(proc, PROC_STATE_READY);
    return proc->pid;
}

static isize sys_wait(const trapframe_t *frame) {
    i32 *status = (i32 *) frame->a0;

    if (list_empty(&curr_proc->children) &&
        list_empty(&curr_proc->zombie_children))
        return -ECHILD;

    if (!list_empty(&curr_proc->zombie_children)) {
        proc_t *proc =
            list_first_entry(&curr_proc->zombie_children, proc_t, tree_node);
        if (status)
            *status = proc->exit_status & 0xFF;
        proc_deinit(proc);
        kfree(proc);
        return proc->pid;
    }

    sched_update_state(curr_proc, PROC_STATE_WAIT_CHILD);

    proc_t *proc = curr_proc->proc_waiting;
    if (status)
        *status = proc->exit_status & 0xFF;
    proc_deinit(proc);
    kfree(proc);
    return proc->pid;
}

static isize sys_reboot(const trapframe_t *frame) {
    switch (frame->a0) {
    case REBOOT_REBOOT: {
#if defined(PLATFORM_QEMU)
        register usize a0 asm("a0") = 1;
        register usize a1 asm("a1") = 0;
        register usize a6 asm("a6") = 0;
        register usize a7 asm("a7") = 0x53525354;
        asm volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a6), "r"(a7));
#elif defined(PLATFORM_MILKV_DUO)
        *((volatile u32 *) 0x03010000) = 0x01;
#else
        return -ENOSYS;
#endif
        break;
    }
    case REBOOT_SHUTDOWN: {
#if defined(PLATFORM_QEMU)
        register usize a0 asm("a0") = 0;
        register usize a1 asm("a1") = 0;
        register usize a6 asm("a6") = 0;
        register usize a7 asm("a7") = 0x53525354;
        asm volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a6), "r"(a7));
#elif defined(PLATFORM_MILKV_DUO)
        register usize a0 asm("a0");
        register usize a1 asm("a1");
        register usize a6 asm("a6") = 1;
        register usize a7 asm("a7") = 0x48534D;
        asm volatile("ecall" : "=r"(a0), "=r"(a1) : "r"(a6), "r"(a7));
#else
        return -ENOSYS;
#endif
        break;
    }
    default:
        return -EINVAL;
    }
    unreachable();
}

isize (*const SYSCALL_TABLE[])(const trapframe_t *) = {
    sys_exit,   sys_read, sys_write, sys_yield,  sys_sleepns,
    sys_getpid, sys_proc, sys_wait,  sys_reboot,
};
const usize NR_SYSCALLS = sizeof(SYSCALL_TABLE) / sizeof(SYSCALL_TABLE[0]);
