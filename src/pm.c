#include <pm.h>

#include <arch.h>

[[noreturn]] void pm_hang(void) {
    while (true) {
        wait_for_intr();
    }
}
