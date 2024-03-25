#include <kprintf.h>

#include <stdarg.h>
#include <types.h>
#include <uart.h>

static usize print_usize(usize val, i32 radix) {
    if (val == 0) {
        uart_putc('0');
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
        uart_putc(buf[i]);
    }

    return sizeof(buf) - pos;
}

static usize print_isize(isize val, i32 radix) {
    usize n = 0;

    if (val < 0) {
        uart_putc('-');
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
            uart_putc(nibble + '0');
        } else {
            uart_putc(nibble - 10 + 'a');
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
            uart_putc(*fmt);
            fmt++;
            n++;
            continue;
        }

        fmt++;
        switch (*fmt) {
        case '%':
            uart_putc('%');
            n++;
            break;
        case 'c':
            uart_putc(va_arg(args, isize));
            n++;
            break;
        case 's': {
            const char *s = va_arg(args, const char *);
            for (; *s; s++) {
                uart_putc(*s);
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
