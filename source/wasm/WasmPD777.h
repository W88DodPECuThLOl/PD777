#pragma once

#include "../core/PD777.h"

class WasmPD777 : public PD777 {
protected:
    // grah
    virtual void present() override;
    // sound
    virtual void setFLS(const s64 clockCounter, const u8 value) override {}
    virtual void setFRS(const s64 clockCounter, const u8 value) override {}
    // input
    virtual bool isPD1() override { return false; }
    virtual bool isPD2() override { return false; }
    virtual bool isPD3() override { return false; }
    virtual bool isPD4() override { return false; }
    virtual u8 readKIN() override { return ~0; }
public:
    virtual void registerDump() override {}
};
