#include "WasmPD777.h"
#include "clangSupport.h"

extern "C" {
WASM_IMPORT("sound", "writeFLS")
void writeFLS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect);

WASM_IMPORT("sound", "writeFRS")
void writeFRS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect);
} // extern "C"

WasmPD777::WasmPD777()
    : PD777()
    , bVRAMDirty(true)
{
    PD777::init();
    keyStatus.clear();
}

void
WasmPD777::present()
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
            }
        }
    }

    setVRAMDirty();
}

void
WasmPD777::readKIN(KeyStatus& key)
{
    key = keyStatus;
}

void
WasmPD777::reset()
{
    crt.reset();
    stack.reset();
    regs.reset();
    sound.reset();
    calledVBLK = false;
}

void
WasmPD777::setFLS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect)
{
    writeFLS(clockCounter, value, reverberatedSoundEffect);
}

void
WasmPD777::setFRS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect)
{
    writeFRS(clockCounter, value, reverberatedSoundEffect);
}
