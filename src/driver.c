#include <driver.h>

#include <config.h>
#include <errno.h>

driver_t *driver_table[DRIVER_TABLE_SIZE];
u32 nr_drivers = 0;

i32 driver_add(driver_t *driver) {
    if (nr_drivers == DRIVER_TABLE_SIZE)
        return -EAGAIN;
    driver_table[nr_drivers] = driver;
    nr_drivers++;
    return 0;
}
