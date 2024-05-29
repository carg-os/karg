#include <kprintf.h>

#include <dev.h>
#include <stdarg.h>
#include <types.h>

/*
static usize print_usize(usize val, i32 radix) {
    if (val == 0) {
        dev_write(DEV_INIT(0, 0), "0", 1);
        return 1;
    }

    char buf[(sizeof(val) * 8 + 2) / 3];
    usize pos = sizeof(buf);

    while (val != 0) {
        pos--;
        u8 digit = val % radix;
        if (digit < 10) {
            buf[pos] = digit + '0';
        } else {
            buf[pos] = digit - 10 + 'a';
        }
        val /= radix;
    }

    for (usize i = pos; i < sizeof(buf); i++) {
        dev_putc(DEV_INIT(0, 0), buf[i]);
    }

    return sizeof(buf) - pos;
}

static usize print_isize(isize val, i32 radix) {
    usize n = 0;

    if (val < 0) {
        dev_putc(DEV_INIT(0, 0), '-');
        n++;
        val = -val;
    }

    n += print_usize(val, radix);

    return n;
}

static usize print_ptr(void *ptr) {
    for (usize i = sizeof(ptr) * 2; i > 0; i--) {
        u8 nibble = (usize) ptr >> ((i - 1) * 4) & 0xF;
        if (nibble < 10) {
            dev_putc(DEV_INIT(0, 0), nibble + '0');
        } else {
            dev_putc(DEV_INIT(0, 0), nibble - 10 + 'a');
        }
    }
    return sizeof(ptr) * 2;
}

isize kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    usize n = 0;

    while (*fmt) {
        if (*fmt != '%') {
            dev_putc(DEV_INIT(0, 0), *fmt);
            fmt++;
            n++;
            continue;
        }

        fmt++;
        switch (*fmt) {
        case '%':
            dev_putc(DEV_INIT(0, 0), '%');
            n++;
            break;
        case 'c':
            dev_putc(DEV_INIT(0, 0), va_arg(args, isize));
            n++;
            break;
        case 's': {
            const char *s = va_arg(args, const char *);
            for (; *s; s++) {
                dev_putc(DEV_INIT(0, 0), *s);
                n++;
            }
            break;
        }
        case 'd':
        case 'i':
            n += print_isize((i32) va_arg(args, isize), 10);
            break;
        case 'o':
            n += print_usize((u32) va_arg(args, usize), 8);
            break;
        case 'x':
            n += print_usize((u32) va_arg(args, usize), 16);
            break;
        case 'u':
            n += print_usize((u32) va_arg(args, usize), 10);
            break;
        case 'p':
            n += print_ptr(va_arg(args, void *));
            break;
        case 'h':
            fmt++;
            switch (*fmt) {
            case 'd':
            case 'i':
                n += print_isize((i16) va_arg(args, isize), 10);
                break;
            case 'o':
                n += print_usize((u16) va_arg(args, usize), 8);
                break;
            case 'x':
                n += print_usize((u16) va_arg(args, usize), 16);
                break;
            case 'u':
                n += print_usize((u16) va_arg(args, usize), 10);
                break;
            case 'h':
                fmt++;
                switch (*fmt) {
                case 'd':
                case 'i':
                    n += print_isize((i8) va_arg(args, isize), 10);
                    break;
                case 'o':
                    n += print_usize((u8) va_arg(args, usize), 8);
                    break;
                case 'x':
                    n += print_usize((u8) va_arg(args, usize), 16);
                    break;
                case 'u':
                    n += print_usize((u8) va_arg(args, usize), 10);
                    break;
                }
                break;
            }
            break;
        case 'l':
            fmt++;
            switch (*fmt) {
            case 'd':
            case 'i':
                n += print_isize((i64) va_arg(args, isize), 10);
                break;
            case 'o':
                n += print_usize((u64) va_arg(args, usize), 8);
                break;
            case 'x':
                n += print_usize((u64) va_arg(args, usize), 16);
                break;
            case 'u':
                n += print_usize((u64) va_arg(args, usize), 10);
                break;
            }
            break;
        case 'z':
            fmt++;
            switch (*fmt) {
            case 'd':
            case 'i':
                n += print_isize(va_arg(args, isize), 10);
                break;
            case 'o':
                n += print_usize(va_arg(args, usize), 8);
                break;
            case 'x':
                n += print_usize(va_arg(args, usize), 16);
                break;
            case 'u':
                n += print_usize(va_arg(args, usize), 10);
                break;
            }
            break;
        }
        fmt++;
    }

    va_end(args);
    return n;
}
*/
