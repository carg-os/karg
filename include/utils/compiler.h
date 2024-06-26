#pragma once

#define section(name) __attribute__((section(#name)))
#define used __attribute__((used))
