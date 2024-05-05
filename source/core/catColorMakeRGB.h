/**
 * @file    catColorMakeRGB.h
 * @brief   RGB値へ変換
 */

#pragma once

#include "catLowBasicTypes.h"

namespace cat {

inline constexpr u32 MakeRGB888(const f32 r, const f32 g, const f32 b)
{
    const u32 a8 = 0;
    const u32 r8 = (u32)(r * 255) << 16;
    const u32 g8 = (u32)(g * 255) << 8;
    const u32 b8 = (u32)(b * 255);
    return a8 | r8 | g8 | b8;
}

inline constexpr u32 MakeRGB888(const u8 r, const u8 g, const u8 b)
{
    const u32 a8 = 0;
    const u32 r8 = (u32)r << 16;
    const u32 g8 = (u32)g << 8;
    const u32 b8 = (u32)b;
    return a8 | r8 | g8 | b8;
}

inline constexpr u32 MakeARGB8888(const f32 r, const f32 g, const f32 b, const f32 a)
{
    const u32 a8 = (u32)(a * 255) << 24;
    const u32 r8 = (u32)(r * 255) << 16;
    const u32 g8 = (u32)(g * 255) << 8;
    const u32 b8 = (u32)(b * 255);
    return a8 | r8 | g8 | b8;
}

inline constexpr u32 MakeARGB8888(const u8 r, const u8 g, const u8 b, const u8 a)
{
    const u32 a8 = (u32)a << 24;
    const u32 r8 = (u32)r << 16;
    const u32 g8 = (u32)g << 8;
    const u32 b8 = (u32)b;
    return a8 | r8 | g8 | b8;
}

} // namespace cat
