#pragma once

#include "../core/PD777.h"
#include "../core/catLowBasicTypes.h"
#include <Windows.h>

class WinPD777 : public PD777 {
    class WinImage* image;
    bool finished = false;
    /**
     * @brief パドル1～パドル4の値
     */
    f32 padValue[4] = {};
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
    virtual bool isPD1(u8& value) override;
    virtual bool isPD2(u8& value) override;
    virtual bool isPD3(u8& value) override;
    virtual bool isPD4(u8& value) override;
    virtual u8 readKIN(const u8 STB) override;
public:
    virtual void registerDump() override;
};
