#include <time.h>

#include <arch/riscv/csr.h>

time_t time(void) { return csr_read(time); }
