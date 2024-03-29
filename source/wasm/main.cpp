#include "WasmPD777.h"
#include "clangSupport.h"
#include "catLowMemory.h"

extern "C" {
WASM_EXPORT void initialize();
WASM_EXPORT void terminate();
/**
 * @brief CPUを実行する
 * @param[in] clock 実行するクロック数
 */
WASM_EXPORT void execute(const s32 clock);
WASM_EXPORT bool setupRom(const void* data, size_t dataSize);
WASM_EXPORT bool setupCGRom(const void* data, size_t dataSize);
WASM_EXPORT bool isVRAMDirty();
WASM_EXPORT void setVRAMDirty();
WASM_EXPORT void* getVRAMImage();
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
execute(const s32 clock)
{
    if(cpu) [[likely]] {
        for(s32 i = 0; i < clock; ++i) {
            cpu->execute();
        }
    }
}

bool
setupRom(const void* data, size_t dataSize)
{
    return cpu && cpu->setupRom(data, dataSize);
}

bool
setupCGRom(const void* data, size_t dataSize)
{
    return cpu && cpu->setupCGRom(data, dataSize);
}

bool
isVRAMDirty()
{
    return cpu->isVRAMDirty();
}

void
setVRAMDirty()
{
    cpu->setVRAMDirty();
}

void*
getVRAMImage()
{
    if(!cpu->isVRAMDirty()) {
        return nullptr;
    }
    cpu->resetVRAMDirty();
    return cpu->getImage();
}
