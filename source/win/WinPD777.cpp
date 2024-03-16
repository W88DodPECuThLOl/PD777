#include "WinPD777.h"
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
    if(mTimeStart.QuadPart != 0) [[likely]] {
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
    }
    QueryPerformanceCounter(&mTimeStart);

    // キー入力の更新
    updateKey();
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
    padValue[0] = clamp(padValue[0] - gamePadState.analogs[0].y * 0.03f, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
    value = (u8)((u8)padValue[0] & 0x7F);
    return true;
}

bool
WinPD777::isPD2(u8& value)
{
    s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    padValue[1] = clamp(padValue[1] + gamePadState.analogs[0].x * 0.03f, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
    value = (u8)((u8)padValue[1] & 0x7F);
    return true;
}

bool
WinPD777::isPD3(u8& value)
{
    s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    padValue[2] = clamp(padValue[2] + gamePadState.analogs[1].x * 0.03f, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
    value = (u8)((u8)padValue[2] & 0x7F);
    return true;
}

bool
WinPD777::isPD4(u8& value)
{
    s32 gamePadIndex = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    padValue[3] = clamp(padValue[3] - gamePadState.analogs[1].y * 0.03f, KeyStatus::PAD_MIN_VALUE, KeyStatus::PAD_MAX_VALUE);
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

void
WinPD777::readKIN(KeyStatus& key)
{
    key = keyStatus;
}

void
WinPD777::updateKey()
{
    keyStatus.clear();

    s32 gamePadIndex = 0;
    static cat::core::pad::GamePadButtonState prevButtons = 0;
    cat::core::pad::GamePadState gamePadState;
    cat::core::pad::getPadState(gamePadIndex, &gamePadState);
    if(!gamePadState.isControllerConnected()) {
        return; // コントローラが接続されていなかった
    }
    const auto current     = gamePadState.buttons;
    const auto edge        = current ^ prevButtons;
    const auto risingEdge  = edge &  current;
    const auto fallingEdge = edge & ~current;
    prevButtons = current;

    // メモ）コーススイッチをデジタルパッドの上下で切り替える
    {
        u8 courseSwitch = getCourseSwitch();
        if(fallingEdge & cat::core::pad::ButtonMask::DPAD_UP) {
            if(courseSwitch < 5) {
                courseSwitch++;
                setCourseSwitch(courseSwitch);
            }
        }
        if(fallingEdge & cat::core::pad::ButtonMask::DPAD_DOWN) {
            if(courseSwitch > 1) {
                courseSwitch--;
                setCourseSwitch(courseSwitch);
            }
        }
    }
/*
    padValue[0] = clamp(padValue[0] - gamePadState.analogs[0].y * 0.03f, PAD_MIN_VALUE, PAD_MAX_VALUE);
    padValue[1] = clamp(padValue[1] + gamePadState.analogs[0].x * 0.03f, PAD_MIN_VALUE, PAD_MAX_VALUE);
    padValue[2] = clamp(padValue[2] + gamePadState.analogs[1].x * 0.03f, PAD_MIN_VALUE, PAD_MAX_VALUE);
    padValue[3] = clamp(padValue[3] - gamePadState.analogs[1].y * 0.03f, PAD_MIN_VALUE, PAD_MAX_VALUE);
*/

    // 
    // PUSH1 : X
    // PUSH2 : B
    // PUSH3 : Y
    // PUSH4 : A
    // 

    const bool start  = current & (cat::core::pad::ButtonMask::START | cat::core::pad::ButtonMask::RIGHT_THUMB);
    const bool leverSwitch1Left = (current & cat::core::pad::ButtonMask::DPAD_LEFT) || (gamePadState.analogs[0].x < -0.3f);
    const bool leverSwitch1Right = (current & cat::core::pad::ButtonMask::DPAD_RIGHT) || (gamePadState.analogs[0].x > 0.3f);
    const bool select = current & (cat::core::pad::ButtonMask::BACK | cat::core::pad::ButtonMask::LEFT_THUMB);
    const bool leverSwitch2Left = current & cat::core::pad::ButtonMask::LEFT_SHOULDER;
    const bool leverSwitch2Right = current & cat::core::pad::ButtonMask::RIGHT_SHOULDER;
    const bool push4 = current & cat::core::pad::ButtonMask::A;
    const bool push3 = current & cat::core::pad::ButtonMask::Y;
    const bool push2 = current & cat::core::pad::ButtonMask::B;
    const bool push1 = current & cat::core::pad::ButtonMask::X;

    // [A08]
    if(start) { keyStatus.setGameStartKey(); }
    if(leverSwitch1Left) { keyStatus.setLeverSwitch1Left(); }
    if(leverSwitch1Right) { keyStatus.setLeverSwitch1Right(); }
    if(select) { keyStatus.setGameSelectKey(); }
    // [A09]
    if(leverSwitch2Left) { keyStatus.setLeverSwitch2Left(); }
    if(leverSwitch2Right) { keyStatus.setLeverSwitch2Right(); }
    // [A10]
    if(push4) { keyStatus.setPush4(); }
    if(push3) { keyStatus.setPush3(); }
    // [A11]
    if(push2) { keyStatus.setPush2(); }
    if(push1) { keyStatus.setPush1(); }
    // [A12]
    keyStatus.setCourseSwitch(getCourseSwitch());

    //
    // パドル
    //

    keyStatus.setPD1((u8)padValue[0]);
    keyStatus.setPD2((u8)padValue[1]);
    keyStatus.setPD3((u8)padValue[2]);
    keyStatus.setPD4((u8)padValue[3]);

    //
    // 特殊
    //

    // ４方向使用するときの上
    if((current & cat::core::pad::ButtonMask::DPAD_UP) || (gamePadState.analogs[0].y > 0.3f)) {
        keyStatus.setUp();
    }
    // ４方向使用するときの下
    if((current & cat::core::pad::ButtonMask::DPAD_DOWN) || (gamePadState.analogs[0].y < -0.3f)) {
        keyStatus.setDown();
    }
    // デバッグモード
    if(false) {
        keyStatus.setDebugMode();
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
WinPD777::setFRS(const s64 clockCounter, const u8 value, const bool reverberatedSoundEffect)
{
    if(auto snd = getSound(clockCounter); snd) {
        snd->setFRS(clockCounter, value, reverberatedSoundEffect);
    }
}

WinPD777::WinPD777()
    : PD777()
    , image(new WinImage())
{
    PD777::init();
}

bool
WinPD777::setup(const std::optional<std::vector<u8>>& codeData, const std::optional<std::vector<u8>>& cgData)
{
    if(codeData && !setupRom(codeData.value().data(), codeData.value().size())) {
        return false; // 設定に失敗した
    }
    if(cgData && !setupCGRom(cgData.value().data(), cgData.value().size())) {
        return false; // 設定に失敗した
    }
    return true;
}

bool
WinPD777::targetDependentSetup(HWND hwnd)
{
    image->make(hwnd);
    // 開始時間の初期化
    mTimeStart.QuadPart = 0;
    // サウンド初期化
    getSound(0);
    return true;
}

void
WinPD777::onPaint(HWND hwnd)
{
    image->onPaint(hwnd);
}
