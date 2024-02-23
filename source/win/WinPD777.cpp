﻿#include "WinPD777.h"
#include "cat/gamePad/catPad.h"
#include "cat/audio/catLowLevelDeviceAudioXAudio2.h"
#include "cat/decoder/cassetteVision/catAudioDecoderCassetteVision.h"

#include <cstdio>

#pragma comment (lib, "Winmm.lib")

namespace {

inline f32 clamp(f32 value, const f32 minValue, const f32 maxValue)
{
    if(value < minValue) { return minValue; }
    if(value > maxValue) { return maxValue; }
    return value;
}

} // namespace

class WinImage {
public:
    static constexpr s32 WIDTH = 75 * PD777::dotWidth;
    static constexpr s32 HEIGHT = 60 * PD777::dotHeight;

    BITMAPINFO bmpInfo {};
    LPDWORD lpPixel;

    HBITMAP hBitmap;
    HDC hMemDC;

    HWND hwnd;

    WinImage()
        : lpPixel()
        , hBitmap()
        , hMemDC()
        , hwnd()
    {
    }

    void make(HWND hwnd)
    {
        this->hwnd = hwnd;

        //DIBの情報を設定する
        bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biWidth=WIDTH;
        bmpInfo.bmiHeader.biHeight=-HEIGHT;
        bmpInfo.bmiHeader.biPlanes=1;
        bmpInfo.bmiHeader.biBitCount=32;
        bmpInfo.bmiHeader.biCompression=BI_RGB;

        //DIBSection作成
        auto hdc = GetDC(hwnd);
        hMemDC = CreateCompatibleDC(hdc);
        if(hMemDC) {
            hBitmap = CreateDIBSection(hdc, &bmpInfo, DIB_RGB_COLORS, (void**)&lpPixel, NULL, 0);
            if(hBitmap) {
                SelectObject(hMemDC,hBitmap);
            }
        }
        ReleaseDC(hwnd,hdc);
    }

    void release() {
        if(hBitmap) {
            DeleteObject(hBitmap);
            hBitmap = 0;
        }
        if(hMemDC) {
            DeleteDC(hMemDC);
            hMemDC = 0;
        }
    }

    void onPaint(HWND hwnd) const
    {
        RECT rect;
        GetClientRect(hwnd, &rect);

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        SetStretchBltMode(hdc, COLORONCOLOR);
        //表画面へ転送
        StretchDIBits(hdc,
            // コピー先
            0,0, rect.right,rect.bottom,
            // コピー元
            0,0, WIDTH, HEIGHT,lpPixel,&bmpInfo,DIB_RGB_COLORS,SRCCOPY);
        EndPaint(hwnd, &ps);
    }

    void update()
    {
        InvalidateRect(hwnd, 0, false);
        UpdateWindow(hwnd);
    }
};

namespace {

cat::core::decoder::CassetteVisionAudioDecoder*
getSound(const std::int64_t clockCounter)
{
    static std::unique_ptr<cat::core::snd::LowLevelDeviceAudioXAudio2> gAudio;
    static cat::core::snd::AudioHandle sound = nullptr;
    static cat::core::decoder::CassetteVisionAudioDecoder* decoder = nullptr;
    if(!sound) {
        gAudio = std::make_unique<cat::core::snd::LowLevelDeviceAudioXAudio2>();
        decoder = new cat::core::decoder::CassetteVisionAudioDecoder();
        decoder->resetInnerClock(clockCounter);
        sound = gAudio->play(decoder, 0);
    }
    return decoder;
}

// 時間調整用
LARGE_INTEGER mTimeStart;

} // namespace

void
WinPD777::present()
{
    // フレームバッファからGDIのビットマップにコピー
    {
        // (10,0)-(84,59)の範囲をGDIにコピーして描画している
        const s32 offsetX = 10 * dotWidth;
        for(int y = 0; y < WinImage::HEIGHT; y++) {
            for(int x = 0; x < WinImage::WIDTH; x++) {
                image->lpPixel[x + y * WinImage::WIDTH] = frameBuffer[x + offsetX + (y*frameBufferWidth)];
            }
        }

        // 光線銃の照準
        if(gun.isEnableGun()) {
            const s32 gunX = gun.getGunX();
            const s32 gunY = gun.getGunY();
            gun.setHit(image->lpPixel[gunX + gunY * WinImage::WIDTH] != 0);

            if(gun.isDrawGunTarget()) {
                for(int x = -2; x < 3; x++) {
                    image->lpPixel[gunX + x + gunY * WinImage::WIDTH] = 0xFF0000;
                }
                for(int y = -2; y < 3; y++) {
                    image->lpPixel[gunX + (gunY + y) * WinImage::WIDTH] = 0xFF0000;
                }
            }
        }
        image->update(); // 画面の更新要求
    }

    // 時間調整
    {
        LARGE_INTEGER mTimeFreq;
        QueryPerformanceFrequency(&mTimeFreq);

        auto FRAME_TIME = 1.0 / 60.0;
        LARGE_INTEGER mTimeEnd;
        QueryPerformanceCounter(&mTimeEnd);
        auto mFrameTime = static_cast<double>(mTimeEnd.QuadPart - mTimeStart.QuadPart) / static_cast<double>(mTimeFreq.QuadPart);
        if (mFrameTime < FRAME_TIME)
        {
            DWORD sleepTime = static_cast<DWORD>((FRAME_TIME - mFrameTime) * 1000);
            timeBeginPeriod(1);
            Sleep(sleepTime);
            timeEndPeriod(1);
        }
        QueryPerformanceCounter(&mTimeStart);
    }
}

void
WinPD777::registerDump()
{
    // メモリ
    for(int j = 0x00; j < 0x10; j++) {
        int i = j;
        auto m0 = readMem((i << 2) | 0);
        auto m1 = readMem((i << 2) | 1);
        auto m2 = readMem((i << 2) | 2);
        auto m3 = readMem((i << 2) | 3);
        std::printf("%02X:%02X %02X %02X %02X | ", i, m0, m1, m2, m3);
        i += 0x10;
        m0 = readMem((i << 2) | 0);
        m1 = readMem((i << 2) | 1);
        m2 = readMem((i << 2) | 2);
        m3 = readMem((i << 2) | 3);
        std::printf("%02X:%02X %02X %02X %02X\x1b[K\n", i, m0, m1, m2, m3);
    }

    // レジスタ
    const auto H = regs.getH();
    const auto L = regs.getL();
    const auto A1 = regs.getA1();
    const auto A2 = regs.getA2();
    const auto A3 = regs.getA3();
    const auto A4 = regs.getA4();
    const auto M  = readMemAtHL();
    const auto STB  = regs.getSTB();
    std::printf("HL:%02X:%X M:%02X STB:%X \x1b[K\n", H, L, M, STB);
    std::printf("A1:%02X A2:%02X A3:%02X A4:%02X \x1b[K\n", A1, A2, A3, A4);
}

bool
WinPD777::isPD1(u8& value)
{
    s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    padValue[0] = clamp(padValue[0] - gamePadState.analogs[0].y * 0.03f, PAD_MIN_VALUE, PAD_MAX_VALUE);
    value = (u8)((u8)padValue[0] & 0x7F);
    return true;
}

bool
WinPD777::isPD2(u8& value)
{
    s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    padValue[1] = clamp(padValue[1] + gamePadState.analogs[0].x * 0.03f, PAD_MIN_VALUE, PAD_MAX_VALUE);
    value = (u8)((u8)padValue[1] & 0x7F);
    return true;
}

bool
WinPD777::isPD3(u8& value)
{
    s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    padValue[2] = clamp(padValue[2] + gamePadState.analogs[1].x * 0.03f, PAD_MIN_VALUE, PAD_MAX_VALUE);
    value = (u8)((u8)padValue[2] & 0x7F);
    return true;
}

bool
WinPD777::isPD4(u8& value)
{
    s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    padValue[3] = clamp(padValue[3] - gamePadState.analogs[1].y * 0.03f, PAD_MIN_VALUE, PAD_MAX_VALUE);
    value = (u8)((u8)padValue[3] & 0x7F);
    return true;
}

s32
WinPD777::Gun::getGunX()
{
    s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    return clamp(gamePadState.analogs[0].x * WinImage::WIDTH * 0.5 + WinImage::WIDTH * 0.5, 5, WinImage::WIDTH - 5);
}

s32
WinPD777::Gun::getGunY()
{
    s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    return clamp(-gamePadState.analogs[0].y * WinImage::HEIGHT * 0.5 + WinImage::HEIGHT * 0.5, 5, WinImage::HEIGHT - 5);
}

bool
WinPD777::Gun::isFire()
{
    s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    return (gamePadState.buttons & (cat::core::pad::ButtonMask::A | cat::core::pad::ButtonMask::B | cat::core::pad::ButtonMask::X | cat::core::pad::ButtonMask::Y));
}

bool
WinPD777::isGunPortLatch(u8& value)
{
    // 呼び出されたら、光線銃を有効にする
    gun.setEnableGun();

    if(gun.isFire()) {
        // 撃った
        value = 0x02;
        if(gun.isHit()) {
            return true;    // 的に当たっていた
        } else {
            return false;   // 外れていた
        }
    } else {
        // 撃ってない
        value = 0x00;
        return false;
    }
}

u8
WinPD777::readKIN(const u8 STB)
{
    s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);

    // 
    // PUSH1 : X
    // PUSH2 : B
    // PUSH3 : Y
    // PUSH4 : A
    // 

    u8 value = (u8)KIN::None;
    switch(STB) {
        case 0xD:
            {
                // ４方向使用するときの上下
                if((gamePadState.buttons & (cat::core::pad::ButtonMask::DPAD_UP | cat::core::pad::ButtonMask::Y)) || (gamePadState.analogs[0].y > 0.3f)) {
                    value |= (u8)KIN::Push3; // 0x02
                }
                if((gamePadState.buttons & (cat::core::pad::ButtonMask::DPAD_DOWN | cat::core::pad::ButtonMask::A)) || (gamePadState.analogs[0].y < -0.3f)) {
                    value |= (u8)KIN::Push4; // 0x04
                }
                if((gamePadState.buttons & (cat::core::pad::ButtonMask::X | cat::core::pad::ButtonMask::Y))) {
                    value |= (u8)0x40; // PUSH1、PUSH3
                }
                if((gamePadState.buttons & (cat::core::pad::ButtonMask::A | cat::core::pad::ButtonMask::B))) {
                    value |= (u8)0x20; // PUSH2 PUSH4
                }
            }
            break;
        default:
        case 0xE:
            {
                if(gamePadState.buttons & (cat::core::pad::ButtonMask::START | cat::core::pad::ButtonMask::RIGHT_THUMB)) {
                    value |= (u8)KIN::GameStartKey; // 0x01
                }
                if(gamePadState.buttons & (cat::core::pad::ButtonMask::BACK | cat::core::pad::ButtonMask::LEFT_THUMB)) {
                    value |= (u8)KIN::GameSelectKey; // 0x08
                }
                if((gamePadState.buttons & cat::core::pad::ButtonMask::DPAD_LEFT) || (gamePadState.analogs[0].x < -0.3f)) {
                    value |= (u8)KIN::LeverSwitchLeft; // 0x02
                }
                if((gamePadState.buttons & cat::core::pad::ButtonMask::DPAD_RIGHT) || (gamePadState.analogs[0].x > 0.3f)) {
                    value |= (u8)KIN::LeverSwitchRight; // 0x04
                }

                // PUSH1、PUSH2、PUSH3、PUSH4
                if(gamePadState.buttons & (cat::core::pad::ButtonMask::A | cat::core::pad::ButtonMask::B | cat::core::pad::ButtonMask::X | cat::core::pad::ButtonMask::Y)) {
                    value |= (u8)KIN::Push2; // 0x20
                }
            }
            break;
        case 0x5:
            {
                u8 courseSwitch = 3;
                switch(courseSwitch) {
                    case 1: // コーススイッチ1
                        value = (u8)0x01;
                        break;
                    case 2: // コーススイッチ2
                        value = (u8)0x02;
                        break;
                    case 3: // コーススイッチ3
                        value = (u8)0x04;
                        break;
                    case 4: // コーススイッチ4
                        value = (u8)0x08;
                        break;
                    case 5: // コーススイッチ5
                        value = (u8)0x10;
                        break;
                    default:
                        value = 0;
                        break;
                }
                // PUSH 1 or 3
                // PUSH 2 or 4
                if(gamePadState.buttons & cat::core::pad::ButtonMask::Y) {
                    value |= (u8)0x40; // PUSH 3
                }
                if(gamePadState.buttons & cat::core::pad::ButtonMask::A) {
                    value |= (u8)0x20; // PUSH 4
                }

                if(gamePadState.buttons & cat::core::pad::ButtonMask::X) {
                    value |= (u8)0x40; // PUSH 1
                }
                if(gamePadState.buttons & cat::core::pad::ButtonMask::B) {
                    value |= (u8)0x20; // PUSH 2
                }
            }
            break;
        case 0xA:
            {
                if(gamePadState.buttons & (cat::core::pad::ButtonMask::START | cat::core::pad::ButtonMask::RIGHT_THUMB)) {
                    value |= (u8)KIN::GameStartKey; // 0x01
                }
                if(gamePadState.buttons & (cat::core::pad::ButtonMask::BACK | cat::core::pad::ButtonMask::LEFT_THUMB)) {
                    value |= (u8)KIN::GameSelectKey; // 0x08
                }

                if((gamePadState.buttons & cat::core::pad::ButtonMask::DPAD_LEFT) || (gamePadState.analogs[0].x < -0.3f)) {
                    value |= (u8)KIN::LeverSwitchLeft; // 0x02
                }
                if((gamePadState.buttons & cat::core::pad::ButtonMask::DPAD_RIGHT) || (gamePadState.analogs[0].x > 0.3f)) {
                    value |= (u8)KIN::LeverSwitchRight; // 0x04
                }
            }
            break;

        case 0xC:
            {
                if(gamePadState.buttons & (cat::core::pad::ButtonMask::START | cat::core::pad::ButtonMask::RIGHT_THUMB)) {
                    value |= (u8)KIN::GameStartKey; // 0x01
                }
                if(gamePadState.buttons & (cat::core::pad::ButtonMask::BACK | cat::core::pad::ButtonMask::LEFT_THUMB)) {
                    value |= (u8)KIN::GameSelectKey; // 0x08
                }
                if((gamePadState.buttons & cat::core::pad::ButtonMask::DPAD_LEFT) || (gamePadState.analogs[0].x < -0.3f)) {
                    value |= (u8)KIN::LeverSwitchLeft; // 0x02
                }
                if((gamePadState.buttons & cat::core::pad::ButtonMask::DPAD_RIGHT) || (gamePadState.analogs[0].x > 0.3f)) {
                    value |= (u8)KIN::LeverSwitchRight; // 0x04
                }

                if(gamePadState.buttons & (cat::core::pad::ButtonMask::X | cat::core::pad::ButtonMask::Y)) {
                    value |= (u8)KIN::Push1;
                }
                if(gamePadState.buttons & (cat::core::pad::ButtonMask::A | cat::core::pad::ButtonMask::B)) {
                    value |= (u8)KIN::Push2;
                }
            }
            break;
    }
    return value & 0x7F;
}

void
WinPD777::updateFLS(const s64 clockCounter, const u8 value)
{
    if(auto snd = getSound(clockCounter); snd) {
        snd->updateFLS(clockCounter, value);
    }
}

void
WinPD777::setFLS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect)
{
    if(auto snd = getSound(clockCounter); snd) {
        snd->setFLS(clockCounter, value, reverberatedSoundEffect);
    }
}

void
WinPD777::updateFRS(const s64 clockCounter, const u8 value)
{
    if(auto snd = getSound(clockCounter); snd) {
        snd->updateFRS(clockCounter, value);
    }
}

void
WinPD777::setFRS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect)
{
    if(auto snd = getSound(clockCounter); snd) {
        snd->setFRS(clockCounter, value, reverberatedSoundEffect);
    }
}

WinPD777::WinPD777(HWND hwnd)
    : PD777()
    , image(new WinImage())
{
    image->make(hwnd);

    // 開始時間の初期化
    QueryPerformanceCounter(&mTimeStart);
}

void
WinPD777::onPaint(HWND hwnd)
{
    image->onPaint(hwnd);
}
