#include <fb.h>

#include <de.h>
#include <driver.h>
#include <font.h>
#include <hdmi.h>

typedef enum {
    NORMAL,
    ESCAPE,
    NUM,
} state_t;

i32 fb_putc(u32 minor, char c);

static driver_t driver = {
    .nr_devs = 1,
    .irqs = nullptr,
    .isr = nullptr,
    .getc = nullptr,
    .putc = fb_putc,
};
u32 framebuffer[1600 * 900 * 2];
static i32 pos = 0, line = 0;
static state_t state = NORMAL;
static i32 num = 0, fg = 0xFFF2EFDE, bg = 0xFF202020;

i32 init_fb(void) {
    i32 res = driver_add(&driver);
    if (res < 0)
        return res;

    int width = 1600;
    int height = 900;
    hdmi_resolution_id_t id = hdmi_best_match(width, height);
    hdmi_init(id);
    de_init(width, height, hdmi_get_screen_width(), hdmi_get_screen_height());
    de_set_active_framebuffer(framebuffer);

    for (int i = 0; i < 1600 * 900 * 2; i++) {
        framebuffer[i] = bg;
    }

    return 0;
}

static void write_fb(i32 i, i32 j, u32 color) {
    framebuffer[1600 * (i + 16 * line) + j + 8 * pos] = color;
    if (line >= 57)
        framebuffer[1600 * (i + 16 * (line - 57)) + j + 8 * pos] = color;
}

i32 fb_putc(u32 minor, char c) {
    (void) minor;

    if (state == NORMAL) {
        if (c == '\n') {
            line++;
            if (line == 112) {
                de_set_active_framebuffer(framebuffer);
                line = 55;
                for (i32 i = 0; i < 4; i++) {
                    for (i32 j = 0; j < 1600; j++) {
                        framebuffer[1600 * (i + 16 * (line + 1)) + j] =
                            0xFF202020;
                    }
                }
            } else if (line > 55) {
                de_set_active_framebuffer(framebuffer +
                                          1600 * 16 * (line - 55));
                for (i32 i = 0; i < 4; i++) {
                    for (i32 j = 0; j < 1600; j++) {
                        framebuffer[1600 * (i + 16 * (line + 1)) + j] =
                            0xFF202020;
                    }
                }
            }
            for (; pos < 200; pos++) {
                for (i32 i = 0; i < 16; i++) {
                    for (i32 j = 0; j < 8; j++) {
                        write_fb(i, j, 0xFF202020);
                    }
                }
            }
            asm volatile(".long 0x0010000B");
            pos = 0;
        } else if (c == '\r') {
            for (i32 i = 0; i < 16; i++) {
                for (i32 j = 0; j < 8; j++) {
                    write_fb(i, j, 0xFF202020);
                }
            }
            asm volatile(".long 0x0010000B");
            pos = 0;
        } else if (c == '\x1B') {
            state = ESCAPE;
        } else if (c == '\b') {
            for (i32 i = 0; i < 16; i++) {
                for (i32 j = 0; j < 8; j++) {
                    write_fb(i, j, 0xFF202020);
                }
            }
            pos--;
            for (i32 i = 0; i < 16; i++) {
                for (i32 j = 0; j < 8; j++) {
                    write_fb(i, j, 0xFFF2EFDE);
                }
            }
            asm volatile(".long 0x0010000B");
        } else {
            for (i32 i = 0; i < 16; i++) {
                for (i32 j = 0; j < 8; j++) {
                    if (font[(u8) c][i] & (1 << j)) {
                        write_fb(i, j, fg);
                    } else {
                        write_fb(i, j, bg);
                    }
                }
            }
            pos++;
            for (i32 i = 0; i < 16; i++) {
                for (i32 j = 0; j < 8; j++) {
                    write_fb(i, j, 0xFFF2EFDE);
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
                de_set_active_framebuffer(framebuffer);
                for (i32 i = 0; i < 1600 * 900; i++)
                    framebuffer[i] = 0xFF202020;
                line = 0;
                state = NORMAL;
            } else {
                state = ESCAPE;
            }
        }
    }

    return 0;
}
