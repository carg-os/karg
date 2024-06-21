#pragma once

[[noreturn]] static inline void hang() {
    while (true)
        ;
}
