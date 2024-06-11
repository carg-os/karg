#pragma once

typedef __UINT8_TYPE__ u8;
typedef __UINT16_TYPE__ u16;
typedef __UINT32_TYPE__ u32;
typedef __UINT64_TYPE__ u64;

#define U8_MAX 255
#define U16_MAX 65535
#define U32_MAX 4294967295U
#define U64_MAX 18446744073709551615U

typedef __INT8_TYPE__ i8;
typedef __INT16_TYPE__ i16;
typedef __INT32_TYPE__ i32;
typedef __INT64_TYPE__ i64;

#define I8_MIN -128
#define I16_MIN -32768
#define I32_MIN (-2147483647 - 1)
#define I64_MIN (-9223372036854775807 - 1)

#define I8_MAX 127
#define I16_MAX 32767
#define I32_MAX 2147483647
#define I64_MAX 9223372036854775807

typedef __UINTPTR_TYPE__ usize;
typedef __INTPTR_TYPE__ isize;

#if __WORDSIZE == 64
#define ISIZE_MIN I64_MIN
#define USIZE_MAX U64_MAX
#define ISIZE_MAX I64_MAX
#else
#define ISIZE_MIN I32_MIN
#define USIZE_MAX U32_MAX
#define ISIZE_MAX I32_MAX
#endif
