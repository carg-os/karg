#include <syscall.h>

#include <errno.h>
#include <kalloc.h>
#include <sched.h>

isize sys_proc(const trapframe_t *frame) {
    void *entry = (void *) frame->a0;
    char **argv = (char **) frame->a1;
    if (!entry || !argv)
        return -EFAULT;

    usize argc = 0;
    while (argv[argc]) {
        argc++;
    }

    proc_t *proc = (proc_t *) kmalloc(sizeof(proc_t));
    if (!proc)
        return -ENOMEM;
    i32 res = proc_init(proc, entry, 0, curr_proc, argc, argv);
    if (res < 0)
        return res;

    sched_update_state(proc, PROC_STATE_READY);
    return proc->pid;
}
