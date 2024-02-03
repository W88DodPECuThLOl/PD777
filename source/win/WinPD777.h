#pragma once

#include "../core/PD777.h"
#include "../core/catLowBasicTypes.h"
#include <Windows.h>

class WinPD777 : public PD777 {
    class WinImage* image;
    bool finished = false;
public:
    WinPD777(HWND hwnd);
    void onPaint(HWND hwnd);
    bool isFinish() const { return finished; }
    void setFinish() { finished = true; }
protected:
    // grah
    virtual void present() override;
    // sound
    virtual void setFLS(const s64 clockCounter, const u8 value) override;
    virtual void setFRS(const s64 clockCounter, const u8 value) override;
    // input
    virtual bool isPD1() override;
    virtual bool isPD2() override;
    virtual bool isPD3() override;
    virtual bool isPD4() override;
    virtual u8 readKIN() override;
public:
    virtual void registerDump() override;
};
