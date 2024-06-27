#include <module/log.h>

#include <config.h>
#include <dev.h>
#include <drivers/tty.h>
#include <module/init.h>
#include <module/module.h>
#include <pm.h>
#include <stdarg.h>
#include <utils/str.h>

MODULE_NAME("log");

static bool inited = false;
static char early_log[LOG_BUF_SIZE];
static usize early_log_tail = 0;

static void write(const char *str, usize size) {
    if (!inited) {
        for (usize i = 0; i < size; i++) {
            early_log[early_log_tail++] = str[i];
        }
        return;
    }
    dev_write(make_dev(TTY_DRIVER, 0), (const u8 *) str, size);
}

static void kvprintf(const char *fmt, va_list args) {
    while (*fmt) {
        switch (*fmt) {
        case '%':
            switch (*++fmt) {
            case 's': {
                const char *str = va_arg(args, const char *);
                write(str, str_len(str));
                break;
            }
            case 'p': {
                usize ptr = va_arg(args, usize);
                char buf[sizeof(usize) * 2];
                for (usize i = sizeof(buf); i > 0; i--) {
                    buf[i - 1] = ((usize) ptr & 0xF) + '0';
                    ptr >>= 4;
                }
                write(buf, sizeof(buf));
            }
            }
            break;
        default:
            write(fmt, 1);
        }
        fmt++;
    }
}

static void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args);
    kvprintf(fmt, args);
    va_end(args);
}

void klogf(log_severity_t severity, const char *fmt, ...) {
    va_list args;
    va_start(args);
    switch (severity) {
    case LOG_SEVERITY_DEBUG:
        kprintf("[\x1B[36mdebug\x1B[0m] ");
        break;
    case LOG_SEVERITY_INFO:
        kprintf("[\x1B[32minfo\x1B[0m] ");
        break;
    case LOG_SEVERITY_WARNING:
        kprintf("[\x1B[33mwarning\x1B[0m] ");
        break;
    case LOG_SEVERITY_ERROR:
        kprintf("[\x1B[31merror\x1B[0m] ");
        break;
    case LOG_SEVERITY_CRITICAL:
        kprintf("[\x1B[43mcritical\x1B[0m] ");
        break;
    case LOG_SEVERITY_PANIC:
        kprintf("[\x1B[41mpanic\x1B[0m] ");
        break;
    }
    kvprintf(fmt, args);
    va_end(args);

    if (severity == LOG_SEVERITY_PANIC)
        pm_hang();
}

static i32 init(void) {
    dev_write(make_dev(TTY_DRIVER, 0), (const u8 *) early_log, early_log_tail);
    inited = true;
    return 0;
}

module_post_init(init);
