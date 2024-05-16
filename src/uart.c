#include <uart.h>

#include <config.h>
#include <driver.h>
#include <font.h>
#include <mem.h>
#include <platform.h>
#include <plic.h>
#include <sem.h>

#define REG(reg) *((volatile u8 *) UART_BASE + UART_REG_SIZE * (reg))
#define THR 0
#define RBR 0
#define IER 1
#define IIR 2
#define FCR 2
#define LSR 5
#ifdef PLATFORM_MILKV_DUO
#define USR 31
#endif

#define IER_ERBFI 0x01

#define LSR_THRE 0x20

static sem_t rx_sem;
static char rx_buf[UART_RX_BUF_SIZE];
static usize rx_head = 0, rx_tail = 0;
static usize cursor_pos = 0;
static driver_t driver = {
    .irq = IRQ_UART,
    .isr = uart_isr,
    .getc = uart_getc,
    .putc = uart_putc,
};

i32 init_uart(void) {
    sem_init(&rx_sem);
    i32 res = driver_add(&driver);
    if (res < 0)
        return res;

    plic_enable_irq(IRQ_UART);
    REG(IER) = IER_ERBFI;
    return 0;
}

void uart_isr(void) {
#ifdef PLATFORM_MILKV_DUO
    if (REG(IIR) == 0xC7) {
        REG(USR);
        return;
    }
#endif

    char c = REG(RBR);

    switch (c) {
    case '\x7F':
        c = '\b';
        break;
    case '\r':
        c = '\n';
        break;
    }

    switch (c) {
    case '\b':
        if (cursor_pos > 0) {
            if (rx_tail > 0) {
                rx_tail--;
            } else {
                rx_tail = UART_RX_BUF_SIZE - 1;
            }

            cursor_pos--;

            uart_putc(0, '\b');
            uart_putc(0, ' ');
            uart_putc(0, '\b');
        }
        break;
    default:
        rx_buf[rx_tail] = c;
        rx_tail++;
        rx_tail %= UART_RX_BUF_SIZE;

        cursor_pos++;
        if (c == '\n') {
            sem_signaln(&rx_sem, cursor_pos);
            cursor_pos = 0;
        }

        uart_putc(0, c);

        break;
    }
}

typedef enum {
    NORMAL,
    ESCAPE,
    NUM,
} state_t;

extern u32 framebuffer[1600 * 900];
static i32 pos = 0;
static i32 line = 0;
static state_t state = NORMAL;
static i32 num = 0;
static u32 fg = 0xFFF2EFDE, bg = 0xFF202020;

i32 uart_putc(u32 minor, char c) {
    (void) minor;

    if (c == '\n')
        uart_putc(0, '\r');

    if (state == NORMAL) {
        if (c == '\n') {
            line++;
            if (line > 55) {
                memcpy(framebuffer, framebuffer + 1600 * 16, 1600 * 880 * 4);
                for (i32 i = 1600 * 880; i < 1600 * 900; i++) {
                    framebuffer[i] = 0xFF202020;
                }
                asm volatile(".long 0x0010000B");
                line--;
            }
        } else if (c == '\r') {
            for (i32 i = 0; i < 16; i++) {
                for (i32 j = 0; j < 8; j++) {
                    framebuffer[1600 * (i + 16 * line) + j + 8 * pos] =
                        0xFF202020;
                }
            }
            asm volatile(".long 0x0010000B");
            pos = 0;
        } else if (c == '\x1B') {
            state = ESCAPE;
        } else if (c == '\b') {
            for (i32 i = 0; i < 16; i++) {
                for (i32 j = 0; j < 8; j++) {
                    framebuffer[1600 * (i + 16 * line) + j + 8 * pos] =
                        0xFF202020;
                }
            }
            pos--;
            for (i32 i = 0; i < 16; i++) {
                for (i32 j = 0; j < 8; j++) {
                    framebuffer[1600 * (i + 16 * line) + j + 8 * pos] =
                        0xFFF2EFDE;
                }
            }
            asm volatile(".long 0x0010000B");
        } else {
            for (i32 i = 0; i < 16; i++) {
                for (i32 j = 0; j < 8; j++) {
                    if (font[(u8) c][i] & (1 << j)) {
                        framebuffer[1600 * (i + 16 * line) + j + 8 * pos] = fg;
                    } else {
                        framebuffer[1600 * (i + 16 * line) + j + 8 * pos] = bg;
                    }
                }
            }
            pos++;
            for (i32 i = 0; i < 16; i++) {
                for (i32 j = 0; j < 8; j++) {
                    framebuffer[1600 * (i + 16 * line) + j + 8 * pos] =
                        0xFFF2EFDE;
                }
            }
            asm volatile(".long 0x0010000B");
        }
    } else if (state == ESCAPE) {
        if ('0' <= c && c <= '9') {
            num = c - '0';
            state = NUM;
        }
    } else {
        if ('0' <= c && c <= '9') {
            num = num * 10 + c - '0';
        } else {
            switch (num) {
            case 0:
                fg = 0xFFF2EFDE;
                bg = 0xFF202020;
                break;
            case 31:
                fg = 0xFFDE382B;
                break;
            case 33:
                fg = 0xFFFFC706;
                break;
            case 36:
                fg = 0xFF2CB5E9;
                break;
            case 90:
                fg = 0xFF808080;
                break;
            }
            if (c == 'm') {
                state = NORMAL;
            } else if (c == 'J') {
                for (i32 i = 0; i < 1600 * 900; i++)
                    framebuffer[i] = 0xFF202020;
                line = 0;
                state = NORMAL;
            } else {
                state = ESCAPE;
            }
        }
    }

    while (!(REG(LSR) & LSR_THRE))
        ;
    REG(THR) = c;

    return 0;
}

i32 uart_getc(u32 minor) {
    (void) minor;

    sem_wait(&rx_sem);
    char c = rx_buf[rx_head];
    rx_head++;
    rx_head %= UART_RX_BUF_SIZE;
    return c;
}
