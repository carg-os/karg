#ifndef PROC_H_
#define PROC_H_

#include <config.h>
#include <fd.h>
#include <list.h>
#include <timer.h>
#include <types.h>

typedef isize pid_t;

#define PROC_FLAG_KERNEL 0x00000001

typedef enum {
    PROC_STATE_INIT,
    PROC_STATE_READY,
    PROC_STATE_CURR,
    PROC_STATE_WAIT_SEM,
    PROC_STATE_WAIT_CHILD,
    PROC_STATE_SLEEP,
    PROC_STATE_ZOMBIE,
} proc_state_t;

typedef struct proc {
    list_node_t node;
    list_node_t tree_node;

    pid_t pid;
    u32 flags;

    struct proc *parent;
    list_node_t children;
    list_node_t zombie_children;

    fd_t fds[PROC_MAX_FD + 1];

    proc_state_t state;
    i32 exit_status;
    timer_t timer;
    struct proc *proc_waiting;

    usize *kstack;
    usize *ustack;
    usize *sp;
} proc_t;

extern proc_t *proc_table[];

i32 proc_init(proc_t *proc, void *entry, u32 flags, proc_t *parent, int argc,
              char *argv[]);
void proc_deinit(proc_t *proc);
void proc_ctx_sw(proc_t *old_proc, proc_t *new_proc);
void proc_adopt(proc_t *parent, proc_t *proc);

#endif // PROC_H_
