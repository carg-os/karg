#include <dev.h>

#include <uart.h>

driver_t DRIVERS[] = {
    (driver_t){
        .getc = uart_getc,
        .putc = uart_putc,
    },
};
const usize NR_DRIVERS = sizeof(DRIVERS) / sizeof(DRIVERS[0]);
