#include <module/init.h>

#include <errno.h>
#include <module/log.h>
#include <module/module.h>

extern init_t _pre_init_start, _pre_init_end;
extern init_t _init_start, _init_end;
extern init_t _post_init_start, _post_init_end;

MODULE_NAME("init");

void init_modules(void) {
    log_info("entering the pre-initialization phase");
    for (init_t *init = &_pre_init_start; init < &_pre_init_end; init++) {
        i32 res = init->init();
        if (res < 0)
            log_panic("%s returned errno %s", init->name, errno_name(-res));
        log_info("initialized %s", init->name);
    }

    log_info("entering the initialization phase");
    for (init_t *init = &_init_start; init < &_init_end; init++) {
        i32 res = init->init();
        if (res < 0)
            log_panic("%s returned errno %s", init->name, errno_name(-res));
        log_info("initialized %s", init->name);
    }

    log_info("entering the post-initialization phase");
    for (init_t *init = &_post_init_start; init < &_post_init_end; init++) {
        i32 res = init->init();
        if (res < 0)
            log_panic("%s returned errno %s", init->name, errno_name(-res));
        log_info("initialized %s", init->name);
    }
}
