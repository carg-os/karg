#pragma once

usize strlen(const char *str) {
    usize len;
    for (len = 0; str[len] != '\0'; len++)
        ;
    return len;
}
