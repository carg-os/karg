#include <time.h>

#include <csr.h>

time_t time(void) { return csr_read(time); }
