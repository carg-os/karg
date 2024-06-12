#include <dev_table.h>

#include <init.h>
#include <string.h>

extern dev_init_t _dev_init_start, _dev_init_end;

static i32 init(void) {
    for (usize i = 0; DEV_TABLE[i].name; i++) {
        const dev_node_t *node = &DEV_TABLE[i];
        for (dev_init_t *init_ptr = &_dev_init_start; init_ptr < &_dev_init_end;
             init_ptr++) {
            if (strcmp(init_ptr->compat, node->name) == 0) {
                i32 res = init_ptr->init(node);
                if (res < 0)
                    return res;
                break;
            }
        }
    }
    return 0;
}

MODULE_POST_INIT(init);
