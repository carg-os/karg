#include <config.h>
#include <dev.h>
#include <driver.h>
#include <drivers/fb.h>
#include <drivers/font.h>
#include <drivers/tty.h>
#include <errno.h>
#include <init.h>
#include <kalloc.h>
#include <module.h>

MODULE_NAME("fbcon");

typedef enum {
    STATE_NORMAL,
    STATE_ESCAPE,
    STATE_NUM,
} state_t;

typedef struct {
    u32 x, y;
} cursor_t;

typedef struct {
    dev_t dev;
    u32 width, height;
    state_t state;
    u8 num;
    cursor_t cursor;
    u32 fg, bg;
} ctrl_blk_t;

static ctrl_blk_t *ctrl_blks[DRIVER_DEV_CAPACITY];

static void write_char(ctrl_blk_t *ctrl_blk, char c) {
    for (i32 y = 0; y < FONT_HEIGHT; y++) {
        for (i32 x = 0; x < FONT_WIDTH; x++) {
            if (FONT[(u8) c][y] & 1 << (FONT_WIDTH - 1 - x)) {
                dev_ioctl(ctrl_blk->dev, FB_WRITE_PIX, ctrl_blk->cursor.x + x,
                          ctrl_blk->cursor.y + y, ctrl_blk->fg);
            } else {
                dev_ioctl(ctrl_blk->dev, FB_WRITE_PIX, ctrl_blk->cursor.x + x,
                          ctrl_blk->cursor.y + y, ctrl_blk->bg);
            }
        }
    }
}

static void handle_char(ctrl_blk_t *ctrl_blk, char c) {
    switch (c) {
    case '\b':
        if (ctrl_blk->cursor.x >= FONT_WIDTH)
            ctrl_blk->cursor.x -= FONT_WIDTH;
        break;
    case '\n':
        ctrl_blk->cursor.x = 0;
        ctrl_blk->cursor.y += FONT_HEIGHT;
        break;
    case '\x1B':
        ctrl_blk->state = STATE_ESCAPE;
        break;
    default: {
        write_char(ctrl_blk, c);
        dev_ioctl(ctrl_blk->dev, FB_FLUSH, ctrl_blk->cursor.y, FONT_HEIGHT);
        ctrl_blk->cursor.x += FONT_WIDTH;
        break;
    }
    }
}

static void fill_fb(ctrl_blk_t *ctrl_blk, u32 color) {
    for (u32 y = ctrl_blk->cursor.y; y < ctrl_blk->cursor.y + ctrl_blk->height;
         y++) {
        for (u32 x = ctrl_blk->cursor.x;
             x < ctrl_blk->cursor.x + ctrl_blk->width; x++) {
            dev_ioctl(ctrl_blk->dev, FB_WRITE_PIX, x, y, color);
        }
    }
}

#define COLOR_BLACK 0xFF202020
#define COLOR_RED 0xFFDE382B
#define COLOR_GREEN 0xFF00A600
#define COLOR_YELLOW 0xFFFFC706
#define COLOR_CYAN 0xFF2CB5E9
#define COLOR_WHITE 0xFFF2EFDE
#define COLOR_BRIGHT_BLACK 0xFF808080

#define DEFAULT_FG COLOR_WHITE
#define DEFAULT_BG COLOR_BLACK

static void set_color(ctrl_blk_t *ctrl_blk) {
    switch (ctrl_blk->num) {
    case 0:
        ctrl_blk->fg = DEFAULT_FG;
        ctrl_blk->bg = DEFAULT_BG;
        break;
    case 31:
        ctrl_blk->fg = COLOR_RED;
        break;
    case 32:
        ctrl_blk->fg = COLOR_GREEN;
        break;
    case 33:
        ctrl_blk->fg = COLOR_YELLOW;
        break;
    case 36:
        ctrl_blk->fg = COLOR_CYAN;
        break;
    case 41:
        ctrl_blk->bg = COLOR_RED;
        break;
    case 43:
        ctrl_blk->bg = COLOR_YELLOW;
        break;
    case 90:
        ctrl_blk->fg = COLOR_BRIGHT_BLACK;
        break;
    }
}

static bool handle_escape_code(ctrl_blk_t *ctrl_blk, char c) {
    switch (c) {
    case 'm':
        set_color(ctrl_blk);
        break;
    case 'J': {
        ctrl_blk->cursor.y = 0;
        fill_fb(ctrl_blk, DEFAULT_BG);
        dev_ioctl(ctrl_blk->dev, FB_FLUSH, 0, ctrl_blk->height);
        break;
    }
    case ';':
        return false;
    }
    return true;
}

static void putc(ctrl_blk_t *ctrl_blk, char c) {
    switch (ctrl_blk->state) {
    case STATE_NORMAL:
        handle_char(ctrl_blk, c);
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
            if (handle_escape_code(ctrl_blk, c)) {
                ctrl_blk->state = STATE_NORMAL;
                break;
            }
            ctrl_blk->state = STATE_ESCAPE;
        }
        break;
    }
}

static isize write(u32 num, const u8 *buf, usize size) {
    if (num >= DRIVER_DEV_CAPACITY || !ctrl_blks[num])
        return -ENXIO;
    ctrl_blk_t *ctrl_blk = ctrl_blks[num];
    for (usize i = 0; i < size; i++) {
        putc(ctrl_blk, buf[i]);
    }
    return size;
}

static const driver_t DRIVER = {
    .read = nullptr,
    .write = write,
    .ioctl = nullptr,
};

static void init_dev(dev_t dev) {
    u32 num = dev.num;
    // if (num >= DRIVER_DEV_CAPACITY)
    //     return -EAGAIN;

    ctrl_blk_t *ctrl_blk = (ctrl_blk_t *) kmalloc(sizeof(ctrl_blk_t));
    // if (!ctrl_blk)
    //     return -ENOMEM;
    ctrl_blks[num] = ctrl_blk;

    ctrl_blk->dev = dev;
    dev_ioctl(dev, FB_GET_SCREEN_INFO, &ctrl_blk->width, &ctrl_blk->height);
    ctrl_blk->state = STATE_NORMAL;
    ctrl_blk->cursor.x = 0;
    ctrl_blk->cursor.y = 0;
    ctrl_blk->fg = DEFAULT_FG;
    ctrl_blk->bg = DEFAULT_BG;

    fill_fb(ctrl_blk, DEFAULT_BG);
    dev_ioctl(dev, FB_FLUSH, 0, ctrl_blk->height);

    tty_register_sink(num, make_dev(DRIVER, num));
}

static i32 init(void) {
    fb_register_dev_probe(init_dev);
    return 0;
}

module_init(init);
