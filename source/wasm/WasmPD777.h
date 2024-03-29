#pragma once

#include "../core/catLowBasicTypes.h"
#include "../core/PD777.h"

class WasmPD777 : public PD777 {
    static constexpr s32 WIDTH = 75 * PD777::dotWidth;
    static constexpr s32 HEIGHT = 60 * PD777::dotHeight;
    bool bVRAMDirty;
    u32 image[WIDTH * HEIGHT];
public:
    WasmPD777();
protected:
    // grah
    virtual void present() override;
    // sound
//    virtual void setFLS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect) override;
//    virtual void setFRS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect) override;
    // input
//    virtual bool isPD1(u8& value) override;
//    virtual bool isPD2(u8& value) override;
//    virtual bool isPD3(u8& value) override;
//    virtual bool isPD4(u8& value) override;
//    virtual bool isGunPortLatch(u8& value) override;
//    virtual void readKIN(KeyStatus& key) override;
public:
//    virtual void registerDump() override;
    void setVRAMDirty() noexcept { bVRAMDirty = true; }
    void resetVRAMDirty() noexcept { bVRAMDirty = false; }
    bool isVRAMDirty() const noexcept { return bVRAMDirty; }
    void* getImage() noexcept { return (void*)image; }
};
