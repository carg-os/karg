#include <init.h>

extern i32 (*_init_start)(void), (*_init_end)(void);

i32 init_modules(void) {
    for (i32 (**init_ptr)(void) = &_init_start; init_ptr < &_init_end;
         init_ptr++) {
        i32 res = (*init_ptr)();
        if (res < 0)
            return res;
    }
    return 0;
}
