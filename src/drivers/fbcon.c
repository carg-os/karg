#include <drivers/fbcon.h>

#include <driver.h>
#include <fb.h>
#include <font.h>

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
    cursor_t cursor;
    u8 num;
    u32 fg, bg;
} fbcon_states_t;

static void write_char(cursor_t cursor, char c) {
    for (i32 y = 0; y < FONT_HEIGHT; y++) {
        for (i32 x = 0; x < FONT_WIDTH; x++) {
            if (font[(u8) c][y] & 1 << (FONT_WIDTH - 1 - x)) {
                write_fb(cursor.x + x, cursor.y + y, fg);
            } else {
                write_fb(cursor.x + x, cursor.y + y, bg);
            }
        }
    }
}

static void handle_char(char c) {
    switch (c) {
    case '\b':
        if (cursor.x > FONT_WIDTH)
            cursor.x -= FONT_WIDTH;
        break;
    case '\n':
        cursor.x = 0;
        cursor.y += FONT_HEIGHT;
        break;
    case '\x1B':
        state = STATE_ESCAPE;
        break;
    default:
        write_char(cursor, c);
        fb_flush(cursor.y, FONT_HEIGHT);
        cursor.x += FONT_WIDTH;
        break;
    }
}

static void fill_fb(cursor_t cursor, u32 width, u32 height, u32 color) {
    for (u32 y = cursor.y; y < cursor.y + height; y++) {
        for (u32 x = cursor.x; x < cursor.x + width; x++) {
            write_fb(x, y, color);
        }
    }
}

void set_color(u8 num) {
    switch (num) {
    case 0:
        fg = DEFAULT_FG;
        bg = DEFAULT_BG;
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
}

static bool handle_escape_code(char c) {
    switch (c) {
    case 'm':
        set_color(num);
        break;
    case 'J':
        cursor.y = 0;
        fb_set_offset(0, 0);
        fill_fb(cursor, fb_width, fb_height, DEFAULT_BG);
        fb_flush(0, fb_height);
        break;
    case ';':
        return false;
    }
    return true;
}

static void fbcon_putc(char c) {
    switch (state) {
    case STATE_NORMAL:
        handle_char(c);
        break;
    case STATE_ESCAPE:
        if ('0' <= c && c <= '9') {
            num = c - '0';
            state = STATE_NUM;
        }
        break;
    case STATE_NUM:
        if ('0' <= c && c <= '9') {
            num = num * 10 + c - '0';
        } else {
            if (handle_escape_code(c)) {
                state = STATE_NORMAL;
            } else {
                state = STATE_ESCAPE;
            }
        }
        break;
    }
}

static isize fbcon_write(u32 minor, const u8 *buf, usize size) {
    (void) minor;
    for (usize i = 0; i < size; i++) {
        fbcon_putc(buf[i]);
    }
    return size;
}

driver_t fbcon_driver = {
    .nr_devs = 1,
    .read = nullptr,
    .write = fbcon_write,
};

i32 init_fbcon(void) {
    i32 res = init_fb(&fb_width, &fb_height);
    if (res < 0)
        return res;

    return 0;
}
