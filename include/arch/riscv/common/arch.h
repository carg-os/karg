#pragma once

static inline void wait_for_intr(void) { asm volatile("wfi"); }
