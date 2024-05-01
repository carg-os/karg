#include <driver.h>

#include <config.h>
#include <errno.h>
#include <types.h>

driver_t *driver_table[DRIVER_TABLE_SIZE];
usize nr_drivers = 0;

i32 driver_add(driver_t *driver) {
    driver_table[nr_drivers] = driver;
    nr_drivers++;
    if (nr_drivers == DRIVER_TABLE_SIZE)
        return -EAGAIN;
    return 0;
}
