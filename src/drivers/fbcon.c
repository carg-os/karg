#include <drivers/fbcon.h>

#include <drivers/fb.h>
#include <drivers/tty.h>
#include <font.h>
#include <platform.h>

#define DEFAULT_FG 0xFFF2EFDE
#define DEFAULT_BG 0xFF202020

typedef enum {
    STATE_NORMAL,
    STATE_ESCAPE,
    STATE_NUM,
} state_t;

typedef struct {
    u32 x, y;
} cursor_t;

typedef struct {
    state_t state;
    u8 num;
    cursor_t cursor;
    u32 fg, bg;
} ctrl_blk_t;

static ctrl_blk_t ctrl_blks[FBCON_NR_DEVS];

static void write_char(u32 num, char c) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    for (i32 y = 0; y < FONT_HEIGHT; y++) {
        for (i32 x = 0; x < FONT_WIDTH; x++) {
            if (font[(u8) c][y] & 1 << (FONT_WIDTH - 1 - x)) {
                fb_write_pix(num, ctrl_blk->cursor.x + x,
                             ctrl_blk->cursor.y + y, ctrl_blk->fg);
            } else {
                fb_write_pix(num, ctrl_blk->cursor.x + x,
                             ctrl_blk->cursor.y + y, ctrl_blk->bg);
            }
        }
    }
}

static void handle_char(u32 num, char c) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    switch (c) {
    case '\b':
        if (ctrl_blk->cursor.x > FONT_WIDTH)
            ctrl_blk->cursor.x -= FONT_WIDTH;
        break;
    case '\n':
        ctrl_blk->cursor.x = 0;
        ctrl_blk->cursor.y += FONT_HEIGHT;
        break;
    case '\x1B':
        ctrl_blk->state = STATE_ESCAPE;
        break;
    default:
        write_char(num, c);
        fb_flush(num, ctrl_blk->cursor.y, FONT_HEIGHT);
        ctrl_blk->cursor.x += FONT_WIDTH;
        break;
    }
}

static void fill_fb(u32 num, u32 width, u32 height, u32 color) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    for (u32 y = ctrl_blk->cursor.y; y < ctrl_blk->cursor.y + height; y++) {
        for (u32 x = ctrl_blk->cursor.x; x < ctrl_blk->cursor.x + width; x++) {
            fb_write_pix(num, x, y, color);
        }
    }
}

void set_color(u32 num) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    switch (ctrl_blk->num) {
    case 0:
        ctrl_blk->fg = DEFAULT_FG;
        ctrl_blk->bg = DEFAULT_BG;
        break;
    case 31:
        ctrl_blk->fg = 0xFFDE382B;
        break;
    case 33:
        ctrl_blk->fg = 0xFFFFC706;
        break;
    case 36:
        ctrl_blk->fg = 0xFF2CB5E9;
        break;
    case 90:
        ctrl_blk->fg = 0xFF808080;
        break;
    }
}

static bool handle_escape_code(u32 num, char c) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    switch (c) {
    case 'm':
        set_color(num);
        break;
    case 'J': {
        ctrl_blk->cursor.y = 0;
        u32 width, height;
        fb_get_size(num, &width, &height);
        fill_fb(num, width, height, DEFAULT_BG);
        fb_flush(num, 0, height);
        break;
    }
    case ';':
        return false;
    }
    return true;
}

static void putc(u32 num, char c) {
    ctrl_blk_t *ctrl_blk = &ctrl_blks[num];
    switch (ctrl_blk->state) {
    case STATE_NORMAL:
        handle_char(num, c);
        break;
    case STATE_ESCAPE:
        if ('0' <= c && c <= '9') {
            ctrl_blk->num = c - '0';
            ctrl_blk->state = STATE_NUM;
        }
        break;
    case STATE_NUM:
        if ('0' <= c && c <= '9') {
            ctrl_blk->num = ctrl_blk->num * 10 + c - '0';
        } else {
            if (handle_escape_code(num, c)) {
                ctrl_blk->state = STATE_NORMAL;
            } else {
                ctrl_blk->state = STATE_ESCAPE;
            }
        }
        break;
    }
}

static isize write(u32 num, const u8 *buf, usize size) {
    for (usize i = 0; i < size; i++) {
        putc(num, buf[i]);
    }
    return size;
}

static driver_t driver = {
    .nr_devs = 1,
    .read = nullptr,
    .write = write,
    .ioctl = nullptr,
};

i32 init_fbcon(void) {
    ctrl_blks[0].fg = DEFAULT_FG;
    ctrl_blks[0].bg = DEFAULT_BG;
    dev_t dev = {.driver = &driver, .num = 0};
    tty_register_sink(0, dev);
    return 0;
}
