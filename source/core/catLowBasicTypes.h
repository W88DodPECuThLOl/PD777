/**
 * @file    catLowBasicTypes.h
 * @brief   基本的な型を定義
 */

#pragma once

#if defined(BUILD_WASM)

// WebAssembly

// 基本的な型
typedef signed   char   s8;
typedef unsigned char   u8;
typedef signed   short  s16;
typedef unsigned short  u16;
typedef signed   int    s32;
typedef unsigned int    u32;
typedef signed   long long s64;
typedef unsigned long long u64;
typedef float    f32;
typedef double   f64;
#if __cpp_char8_t
typedef char8_t       c8;
#else
typedef char          c8;
#endif // __cpp_char8_t
typedef char16_t c16;
typedef char32_t c32;

#if BUILD_WASM == 32
typedef unsigned long size_t; // --target = wasm32
#elif BUILD_WASM == 64
typedef unsigned long long size_t; // --target = wasm64
#else
static_assert(false) // 未対応
#endif

#elif defined(PICO_BUILD) && !!PICO_BUILD

// Raspberry Pi Pico/Pico2

#include <cstdint>

// 基本的な型
typedef std::int8_t   s8;
typedef std::uint8_t  u8;
typedef std::int16_t  s16;
typedef std::uint16_t u16;
typedef std::int32_t  s32;
typedef std::uint32_t u32;
typedef std::int64_t  s64;
typedef std::uint64_t u64;
typedef float         f32;
typedef double        f64;
#if __cpp_char8_t
typedef char8_t       c8;
#else
typedef char          c8;
#endif // __cpp_char8_t
typedef char16_t      c16;
typedef char32_t      c32;

#if defined(PICO_32BIT) && !!PICO_32BIT
// 32bit
typedef std::uint32_t size_t;
#else
// 64bit
typedef std::uint64_t size_t;
#endif

#else

// VC++
// Linux, macOS, etc.

#include <cstdint>
#include <stdlib.h>

// 基本的な型
typedef std::int8_t   s8;
typedef std::uint8_t  u8;
typedef std::int16_t  s16;
typedef std::uint16_t u16;
typedef std::int32_t  s32;
typedef std::uint32_t u32;
typedef std::int64_t  s64;
typedef std::uint64_t u64;
typedef float         f32;
typedef double        f64;
#if __cpp_char8_t
typedef char8_t       c8;
#else
typedef char          c8;
#endif // __cpp_char8_t
typedef char16_t      c16;
typedef char32_t      c32;

#endif

static_assert(sizeof(s8)  == 1);
static_assert(sizeof(u8)  == 1);
static_assert(sizeof(s16) == 2);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(s32) == 4);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(s64) == 8);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(c8)  == 1);
static_assert(sizeof(c16) == 2);
static_assert(sizeof(c32) == 4);
static_assert(sizeof(size_t) == sizeof(void*));
