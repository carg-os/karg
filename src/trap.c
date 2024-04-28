#include <trap.h>

#include <errno.h>
#include <platform.h>
#include <plic.h>
#include <rv.h>
#include <syscall.h>
#include <timer.h>
#include <uart.h>

void trap_handler(trapframe_t *frame) {
    if (frame->scause & CSR_SCAUSE_INTR) {
        switch (frame->scause & ~CSR_SCAUSE_INTR) {
        case 5:
            timer_handle_intr();
            break;
        case 9:
            u32 irq = plic_claim();
            switch (irq) {
            case IRQ_UART:
                uart_handle_intr();
                break;
            }
            plic_complete(irq);
            break;
        }
    } else {
        switch (frame->scause) {
        case 8:
            frame->sepc += RV_INSTR_LEN;
            if (frame->a7 >= NR_SYSCALLS) {
                frame->a0 = -ENOSYS;
                break;
            }
            frame->a0 = SYSCALL_TABLE[frame->a7](frame);
            break;
        }
    }
}
