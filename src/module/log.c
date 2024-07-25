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
static char log_buf[LOG_BUF_SIZE];
static usize log_head, log_tail = 0;

static void write(const char *str, usize size) {
    if (!inited) {
        for (usize i = 0; i < size; i++) {
            log_buf[log_tail++] = str[i];
            log_tail %= LOG_BUF_SIZE;
            if (log_tail == log_head) {
                log_head++;
                log_head %= LOG_BUF_SIZE;
            }
        }
        return;
    }
    dev_write(make_dev(TTY_DRIVER, 0), (const u8 *) str, size);
}

static void flush(void) {
    dev_t tty_dev = make_dev(TTY_DRIVER, 0);
    dev_write(tty_dev, (const u8 *) log_buf + log_head,
              LOG_BUF_SIZE - log_head);
    if (log_tail < log_head) {
        dev_write(tty_dev, (const u8 *) log_buf, log_tail);
    }
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
                usize addr = va_arg(args, usize);
                char buf[16];
                for (int i = 0; i < 16; i++) {
                    u8 nibble = (addr >> (i * 4)) & 0xF;
                    if (nibble < 10) {
                        buf[15 - i] = nibble + '0';
                    } else {
                        buf[15 - i] = nibble - 10 + 'A';
                    }
                }
                write(buf, 16);
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

    if (severity == LOG_SEVERITY_PANIC) {
        flush();
        pm_hang();
    }
}

static i32 init(void) {
    flush();
    inited = true;
    return 0;
}

module_post_init(init);
