#pragma once

#include "../core/PD777.h"

class WinPD777 : public PD777 {
protected:
    // grah
    virtual void present() override;
    // sound
    virtual void setFLS(const u8 value) override {}
    virtual void setFRS(const u8 value) override {}
    // input
    virtual bool isPD1() override;
    virtual bool isPD2() override;
    virtual bool isPD3() override;
    virtual bool isPD4() override;
    virtual u8 readKIN() override;
public:
    virtual void registerDump() override;
};
