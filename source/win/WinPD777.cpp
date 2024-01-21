#include "WinPD777.h"
#include "cat/gamePad/catPad.h"
#include "cat/audio/catLowLevelDeviceAudioXAudio2.h"
#include "cat/decoder/cassetteVision/catAudioDecoderCassetteVision.h"

#include <cstdio>

#pragma comment (lib, "Winmm.lib")

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


class Console {
    int renderIndex;
    HANDLE srceenHandle[2];
    CHAR_INFO buffer[128*128];
public:
    void init() {
        renderIndex = 0;
        srceenHandle[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        srceenHandle[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

        CONSOLE_CURSOR_INFO consoleCursorInfo;
        consoleCursorInfo.dwSize = 1;
        consoleCursorInfo.bVisible = FALSE;
        for(auto sh : srceenHandle) { SetConsoleCursorInfo(sh, &consoleCursorInfo); }

        //バッファーを初期化
        for(auto& buf : buffer) {
            buf.Char.AsciiChar = 0x20;
            buf.Attributes = 0x00;
        }
    }

    HANDLE getRenderTarget()
    {
        return srceenHandle[renderIndex];
    }

    CHAR_INFO* getScreenBuffer()
    {
        return buffer;
    }

    void swapRenderTarget() {
        renderIndex = renderIndex ^ 1;
    }

    void cls() {
        auto target = getRenderTarget();
    }

    void Write() {
        auto target = getRenderTarget();
        COORD bufferCoord = { 0, 0 };
        COORD bufferSize = { 128, 128 };
        SMALL_RECT rect = { 0, 0, 80, 60 };
        ::WriteConsoleOutputA(target, buffer, bufferSize, bufferCoord, &rect);
    }

    void present()
    {
        auto target = getRenderTarget();
        SetConsoleActiveScreenBuffer(target);
        swapRenderTarget();
    }
};

// 時間調整用
LARGE_INTEGER mTimeStart;

Console*
getConsole()
{
    static std::unique_ptr<Console> console;
    if(!console) {
        console = std::make_unique<Console>();
        console->init();

        // 開始時間の初期化
        QueryPerformanceCounter(&mTimeStart);
    }
    return console.get();
}

} // namespace

void
WinPD777::present()
{
#if false
    std::printf("\x1b[100A"); // 画面クリア
    for(int y = 0; y < 60; y++) {
        for(int x = 0; x < 80; x++) {
            // 色設定
            switch(frameBuffer[x + (y*frameBufferWidth)]) {
                case 0x00: std::printf("\x1b[48;2;0;0;0m"); break;
                case 0x01: std::printf("\x1b[48;2;100;196;255m"); break;     // 青のインベーダーの色、シェルターの色
                case 0x02: std::printf("\x1b[48;2;170;223;8m"); break;  // 緑のインベーダーの色
                case 0x03: std::printf("\x1b[48;2;0;255;255m"); break;
                case 0x04: std::printf("\x1b[48;2;255;0;0m"); break;
                case 0x05: std::printf("\x1b[48;2;255;0;255m"); break;
                case 0x06: std::printf("\x1b[48;2;255;183;44m"); break;  // 黄のインベーダーの色
                case 0x07: std::printf("\x1b[48;2;255;255;255m"); break;

                case 0x08: std::printf("\x1b[48;2;0;0;0m"); break;
                case 0x09: std::printf("\x1b[48;2;126;192;255m"); break; // 残機の色
                case 0x0A: std::printf("\x1b[48;2;141;229;59m"); break;  // ゲームNo.(レベル)の色
                case 0x0B: std::printf("\x1b[48;2;74;232;139m"); break;  // 得点の色
                case 0x0C: std::printf("\x1b[48;2;255;0;0m"); break;
                case 0x0D: std::printf("\x1b[48;2;255;0;255m"); break;
                case 0x0E: std::printf("\x1b[48;2;255;255;0m"); break;
                case 0x0F: std::printf("\x1b[48;2;255;255;255m"); break;
            }
            // 1マス描画
            std::printf(" ");
        }
	    std::printf("\x1b[38;2;255;255;255m"); std::printf("\x1b[48;2;0;0;0m"); // 黒地に白戻しておく
        std::printf("\x1b[K\n"); // 行末までクリア
    }
#else
    auto console = getConsole();
    console->cls();
    CHAR_INFO* screenBuffer = console->getScreenBuffer();
    for(int y = 0; y < 60; y++) {
        for(int x = 0; x < 80; x++) {
            auto& ch = screenBuffer[x + (y*frameBufferWidth)];
            // 色設定
            switch(frameBuffer[x + (y*frameBufferWidth)]) {
                case 0x00: ch.Attributes = 0x00; break; // std::printf("\x1b[48;2;0;0;0m"); break;
                case 0x01: ch.Attributes = 0x10; break; // std::printf("\x1b[48;2;100;196;255m"); break;     // 青のインベーダーの色、シェルターの色
                case 0x02: ch.Attributes = 0x20; break; // std::printf("\x1b[48;2;170;223;8m"); break;  // 緑のインベーダーの色
                case 0x03: ch.Attributes = 0x30; break; // std::printf("\x1b[48;2;0;255;255m"); break;
                case 0x04: ch.Attributes = 0x40; break; // std::printf("\x1b[48;2;255;0;0m"); break;
                case 0x05: ch.Attributes = 0x50; break; // std::printf("\x1b[48;2;255;0;255m"); break;
                case 0x06: ch.Attributes = 0x60; break; // std::printf("\x1b[48;2;255;183;44m"); break;  // 黄のインベーダーの色
                case 0x07: ch.Attributes = 0x70; break; // std::printf("\x1b[48;2;255;255;255m"); break;

                case 0x08: ch.Attributes = 0x00; break; // std::printf("\x1b[48;2;0;0;0m"); break;
                case 0x09: ch.Attributes = 0x90; break; // std::printf("\x1b[48;2;126;192;255m"); break; // 残機の色
                case 0x0A: ch.Attributes = 0xA0; break; // std::printf("\x1b[48;2;141;229;59m"); break;  // ゲームNo.(レベル)の色
                case 0x0B: ch.Attributes = 0xB0; break; // std::printf("\x1b[48;2;74;232;139m"); break;  // 得点の色
                case 0x0C: ch.Attributes = 0xC0; break; // std::printf("\x1b[48;2;255;0;0m"); break;
                case 0x0D: ch.Attributes = 0xD0; break; // std::printf("\x1b[48;2;255;0;255m"); break;
                case 0x0E: ch.Attributes = 0xE0; break; // std::printf("\x1b[48;2;255;255;0m"); break;
                case 0x0F: ch.Attributes = 0xF0; break; // std::printf("\x1b[48;2;255;255;255m"); break;
            }
            ch.Char.AsciiChar = ' ';
        }
    }
    console->Write();

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

    console->present();
#endif
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
WinPD777::isPD1()
{
	s32 gamePadIndex = 0;
	cat::core::pad::GamePadState gamePadState;
	cat::core::pad::getPadState(gamePadIndex, &gamePadState);
	return (gamePadState.buttons & (cat::core::pad::ButtonMask::X | cat::core::pad::ButtonMask::DPAD_LEFT)) != 0;
}

bool
WinPD777::isPD2()
{
	s32 gamePadIndex = 0;
	cat::core::pad::GamePadState gamePadState;
	cat::core::pad::getPadState(gamePadIndex, &gamePadState);
	return (gamePadState.buttons & (cat::core::pad::ButtonMask::B | cat::core::pad::ButtonMask::DPAD_RIGHT)) != 0;
}

bool
WinPD777::isPD3()
{
	s32 gamePadIndex = 0;
	cat::core::pad::GamePadState gamePadState;
	cat::core::pad::getPadState(gamePadIndex, &gamePadState);
	return (gamePadState.buttons & (cat::core::pad::ButtonMask::Y | cat::core::pad::ButtonMask::DPAD_UP)) != 0;
}

bool
WinPD777::isPD4()
{
	s32 gamePadIndex = 0;
	cat::core::pad::GamePadState gamePadState;
	cat::core::pad::getPadState(gamePadIndex, &gamePadState);
	return (gamePadState.buttons & (cat::core::pad::ButtonMask::A | cat::core::pad::ButtonMask::DPAD_DOWN)) != 0;
}

u8
WinPD777::readKIN()
{
	s32 gamePadIndex = 0;
	cat::core::pad::GamePadState gamePadState;
	cat::core::pad::getPadState(gamePadIndex, &gamePadState);

	u8 value = (u8)KIN::None;
	if(gamePadState.buttons & (cat::core::pad::ButtonMask::START | cat::core::pad::ButtonMask::RIGHT_THUMB)) {
		value |= (u8)KIN::GameStartKey;
	}
	if(gamePadState.buttons & (cat::core::pad::ButtonMask::BACK | cat::core::pad::ButtonMask::LEFT_THUMB)) {
		value |= (u8)KIN::GameSelectKey;
	}
	if((gamePadState.buttons & cat::core::pad::ButtonMask::DPAD_LEFT) || (gamePadState.analogs[0].x < -0.3f)) {
		value |= (u8)KIN::LeverSwitchLeft;
	}
	if((gamePadState.buttons & cat::core::pad::ButtonMask::DPAD_RIGHT) || (gamePadState.analogs[0].x > 0.3f)) {
		value |= (u8)KIN::LeverSwitchRight;
	}
	if(gamePadState.buttons & (cat::core::pad::ButtonMask::A | cat::core::pad::ButtonMask::B | cat::core::pad::ButtonMask::X | cat::core::pad::ButtonMask::Y)) {
		value |= (u8)KIN::PushAll;
	}
	return value & 0x7F;
}

void
WinPD777::setFLS(const s64 clockCounter, const u8 value)
{
    if(auto snd = getSound(clockCounter); snd) {
        snd->setFLS(clockCounter, value);
    }
}

void
WinPD777::setFRS(const s64 clockCounter, const u8 value)
{
    if(auto snd = getSound(clockCounter); snd) {
        snd->setFRS(clockCounter, value);
    }
}
