#include <utils/str.h>

i32 str_cmp(const char *lhs, const char *rhs) {
    while (*lhs && *lhs == *rhs) {
        lhs++;
        rhs++;
    }
    return *lhs - *rhs;
}

usize str_len(const char *str) {
    usize len = 0;
    while (*str) {
        str++;
        len++;
    }
    return len;
}
