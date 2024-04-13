#include "WasmPD777.h"
#include "clangSupport.h"
#include "catLowMemory.h"

/**
 * K1  K2  K3  K4  K5  K6  K7
 * ----------------------------------------
 * STA L1L L1R SEL AUX 6   7    | [A08]
 * 1   L2L L2R 4   5   6   7    | [A09]
 * 1   2   3   4   5   P4  P3   | [A10]
 * 1   2   3   4   5   P2  P1   | [A11]
 * LL  L   C   R   RR  6   7    | [A12]
 */
enum class KeyStateIndex : s32 {
    A08 = 0,
    A09 = 1,
    A10 = 2,
    A11 = 3,
    A12 = 4,

    KeyStateIndexMax
};

enum class KeyMappingIndex : s32 {
    A8  = 0,
    A9,
    A10,
    A11,
    A12,

    B9,
    B10,
    B11,
    B12,
    B13,
    B14,
    B15,

    KeyMappingIndexMax
};

extern "C" {
WASM_EXPORT void initialize();
WASM_EXPORT void terminate();
WASM_EXPORT void reset();
/**
 * @brief CPUを実行する
 * @param[in] clock 実行するクロック数
 */
WASM_EXPORT void execute(const s32 clock);
WASM_EXPORT bool setupCode(const void* data, const size_t dataSize);
WASM_EXPORT bool setupPattern(const void* data, const size_t dataSize);
WASM_EXPORT bool setupAuto(const void* data, const size_t dataSize);
WASM_EXPORT bool isVRAMDirty();
WASM_EXPORT void setVRAMDirty();
WASM_EXPORT const void* getVRAMImage();

WASM_EXPORT void clearKeyStatus();
WASM_EXPORT void setKeyStatus(const KeyStateIndex keyStateIndex, const u8 value);
WASM_EXPORT void setKeyMapping(const KeyMappingIndex keyMappingIndex, const u8 value);

WASM_EXPORT void* memoryAllocate(const s32 size);
WASM_EXPORT void memoryFree(u8* ptr);
} // extern "C"

namespace {
WasmPD777* cpu = nullptr;
} // namespace

void
initialize()
{
    if(cpu) [[unlikely]] {
        terminate();
    }
    cpu = new WasmPD777();
    cpu->init();
}

void
terminate()
{
    if(cpu) [[likely]] {
        cpu->term();
        delete cpu;
        cpu = nullptr;
    }
}

void
reset()
{
    if(cpu) [[likely]] {
        cpu->reset();
    }
}

void
execute(const s32 clock)
{
    if(cpu) [[likely]] {
        for(s32 i = 0; i < clock; ++i) {
            cpu->execute();
        }
    }
}

bool
setupAuto(const void* data, const size_t dataSize)
{
    if(setupPattern(data, dataSize)) {
        return true; // 成功
    }
    if(setupCode(data, dataSize)) {
        reset();
        return true; // 成功
    }
    return false;
}

bool
setupCode(const void* data, const size_t dataSize)
{
    return cpu && cpu->setupCode(data, dataSize);
}

bool
setupPattern(const void* data, const size_t dataSize)
{
    return cpu && cpu->setupPattern(data, dataSize);
}

bool
isVRAMDirty()
{
    return cpu && cpu->isVRAMDirty();
}

void
setVRAMDirty()
{
    if(cpu) [[likely]] {
        cpu->setVRAMDirty();
    }
}

const void*
getVRAMImage()
{
    if(!cpu->isVRAMDirty()) {
        return nullptr;
    }
    cpu->resetVRAMDirty();
    return cpu->getImage();
}

void
clearKeyStatus()
{
    if(!cpu) [[unlikely]] {
        return;
    }
    if(auto keyStatus = cpu->getKeyStatus(); keyStatus) [[likely]] {
        keyStatus->clear();
    }
}

void
setKeyStatus(const KeyStateIndex keyStateIndex, const u8 value)
{
    if(!cpu) [[unlikely]] {
        return;
    }
    if(auto keyStatus = cpu->getKeyStatus(); keyStatus) [[likely]] {
        if(keyStateIndex < KeyStateIndex::KeyStateIndexMax) {
            keyStatus->a[(s32)keyStateIndex] = value & 0x7F;
        } else {
            if(value & 0x80) { keyStatus->setDebugMode(); }
            if(value & 0x40) { keyStatus->setUp(); }
            if(value & 0x20) { keyStatus->setDown(); }
        }
    }
}

void
setKeyMapping(const KeyMappingIndex keyMappingIndex, const u8 value)
{
    if(!cpu || (keyMappingIndex >= KeyMappingIndex::KeyMappingIndexMax)) [[unlikely]] {
        return;
    }
    if(auto keyMapping = cpu->getkeyMapping(); keyMapping) [[likely]] {
        if(keyMappingIndex < KeyMappingIndex::B9) {
            keyMapping->keyMap[(s32)keyMappingIndex] = value;
        } else {
            keyMapping->bitMap[(s32)keyMappingIndex - (s32)KeyMappingIndex::B9] = value;
        }
    }
}

void*
memoryAllocate(const s32 size)
{
    return new u8[size];
}




void
memoryFree(u8* ptr)
{
    delete[] ptr;
}
