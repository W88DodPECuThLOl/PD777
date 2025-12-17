#include <cstdio>
#include "LibretroPD777.h"
#include "libretro.h"

extern retro_video_refresh_t video_cb;

namespace {

    inline f32 clamp(f32 value, const f32 minValue, const f32 maxValue)
    {
        if(value < minValue) { return minValue; }
        if(value > maxValue) { return maxValue; }
        return value;
    }

} // namespace

LibretroPD777::LibretroPD777()
    : PD777()
      , bVRAMDirty(true)
{
    PD777::init();
    keyStatus.clear();
};


void LibretroPD777::present()
{
    // フレームバッファからビットマップにコピー
    {
        // (10,0)-(84,59)の範囲をコピーして描画している
        const s32 offsetX = 10 * dotWidth;
        for(int y = 0; y < HEIGHT; y++) {
            for(int x = 0; x < WIDTH; x++) {
                const auto bgr = frameBuffer[x + offsetX + (y*frameBufferWidth)];
                const auto r = (bgr >> 16) & 0x0000FF;
                const auto b = (bgr << 16) & 0xFF0000;
                const auto g = (bgr      ) & 0x00FF00;
                const auto argb  = 0xFF000000 | r | g | b;
                image[x + y * WIDTH] = argb;
                //image[x + y * WIDTH] = 0xFF44FF88;

            }
        }
    }
    int VIDEO_PITCH = WIDTH * 4; // TODO (mittonk)

    video_cb(reinterpret_cast<uint8_t*>(image), WIDTH, HEIGHT, VIDEO_PITCH);

    setVRAMDirty();
}

void LibretroPD777::readKIN(KeyStatus& key)
{
    key = keyStatus;
}

void LibretroPD777::reset()
{
    crt.reset();
    stack.reset();
    regs.reset();
    sound.reset();
    calledVBLK = false;
} 

bool LibretroPD777::isPD1(u8& value) {
    padValue[0] = clamp(padValue[0] + float(analogStatus.input_analog_left_y[0]) / analogscale, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
    value = (u8)((u8)padValue[0] & 0x7F);
    return true;
}

bool LibretroPD777::isPD2(u8& value)
{
    padValue[1] = clamp(padValue[1] + float(analogStatus.input_analog_left_x[0]) / analogscale, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
    value = (u8)((u8)padValue[1] & 0x7F);
    return true;
}

bool LibretroPD777::isPD3(u8& value)
{
    padValue[2] = clamp(padValue[2] + float(analogStatus.input_analog_left_x[1]) / analogscale, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
    value = (u8)((u8)padValue[2] & 0x7F);
    return true;
}

bool LibretroPD777::isPD4(u8& value)
{
    padValue[3] = clamp(padValue[3] + float(analogStatus.input_analog_left_y[1]) / analogscale, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
    value = (u8)((u8)padValue[3] & 0x7F);
    return true;
}

void LibretroPD777::setFLS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect)
{
    // 再生する音の周波数は「15734.0 / (FLS - 1)」(2以上) で計算できる。

    catAudio->writeFLS(value, reverberatedSoundEffect);
    // TODO (mittonk): clockCounter?
}

void LibretroPD777::setFRS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect)
{
    // TODO (mittonk): Audio processor.
    catAudio->writeFRS(value, reverberatedSoundEffect);
}
