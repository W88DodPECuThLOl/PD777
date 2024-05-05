/**
 * @file    catColorHSVtoRGB.h
 * @brief   HSVをRGBへ変換
 */

#pragma once

#include "catLowBasicTypes.h"
#include "catColorMakeRGB.h"

namespace cat {

/**
 * @breif   HSV色空間をRGB888に変換する
 * @param[in]   hue         色相(0～1)
 * @param[in]   saturation  彩度(0～1)
 * @param[in]   value       明度(0～1)
 * @return 変換されたRGB888の値
 */
inline constexpr u32 convertHSVtoRGB888(const f32 hue, const f32 saturation, const f32 value)
{
    f32 r = value;
    f32 g = value;
    f32 b = value;
    if (saturation > 0.0f) {
        const f32 h = hue * 6;
        const u32 i = (u32)h;
        const f32 f = h - (f32)i;
        switch (i) {
            default:
            case 0:
                g *= 1 - saturation * (1 - f);
                b *= 1 - saturation;
                break;
            case 1:
                r *= 1 - saturation * f;
                b *= 1 - saturation;
                break;
            case 2:
                r *= 1 - saturation;
                b *= 1 - saturation * (1 - f);
                break;
            case 3:
                r *= 1 - saturation;
                g *= 1 - saturation * f;
                break;
            case 4:
                r *= 1 - saturation * (1 - f);
                g *= 1 - saturation;
                break;
            case 5:
                g *= 1 - saturation;
                b *= 1 - saturation * f;
                break;
        }
    }
    return MakeRGB888(r, g, b);
}

} // namespace cat
