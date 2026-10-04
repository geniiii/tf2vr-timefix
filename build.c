#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <minhook/MinHook.c>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#define SIMDSC_STATIC 1
#define SIMDSC_IMPLEMENTATION
#include <simdscanner.h>

typedef int32_t        i32;
typedef int64_t        i64;
typedef uint8_t        u8;
typedef uint32_t       u32;
typedef uint64_t       u64;
typedef simdsc_string8 String8;

#define MacroConcatImpl(x, y) x##y
#define MacroConcat(x, y)     MacroConcatImpl(x, y)
#define Pad(size)             u8 MacroConcat(_pad, __COUNTER__)[size]

#include "util.c"
#include "hook.c"
#include "main.c"
