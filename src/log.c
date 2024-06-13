#include <log.h>

#include <config.h>
#include <dev.h>
#include <driver.h>
#include <init.h>
#include <module.h>
#include <stdarg.h>
#include <string.h>

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
    dev_write(make_dev(tty_driver, 0), (const u8 *) str, size);
}

static void kvprintf(const char *fmt, va_list args) {
    while (*fmt) {
        switch (*fmt) {
        case '%':
            switch (*++fmt) {
            case 's': {
                const char *str = va_arg(args, const char *);
                write(str, strlen(str));
                break;
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
        kprintf("[\e[36mdebug\e[0m] ");
        break;
    case LOG_SEVERITY_INFO:
        kprintf("[\e[32minfo\e[0m] ");
        break;
    case LOG_SEVERITY_WARNING:
        kprintf("[\e[33mwarning\e[0m] ");
        break;
    case LOG_SEVERITY_ERROR:
        kprintf("[\e[31merror\e[0m] ");
        break;
    case LOG_SEVERITY_CRITICAL:
        kprintf("[\e[37;43mcritical\e[0m] ");
        break;
    case LOG_SEVERITY_PANIC:
        kprintf("[\e[37;41mpanic\e[0m] ");
        break;
    }
    kvprintf(fmt, args);
    va_end(args);
}

static i32 init(void) {
    dev_write(make_dev(tty_driver, 0), (const u8 *) early_log, early_log_tail);
    inited = true;
    return 0;
}

module_post_init(init);
