#include "WinPD777.h"
#include "catPad.h"

#include <cstdio>

void
WinPD777::present()
{
    printf("\x1b[100A"); // 画面クリア
    for(int y = 0; y < 60; y++) {
        for(int x = 0; x < 80; x++) {
            // 色設定
            switch(frameBuffer[x + (y*frameBufferWidth)]) {
                case 0x00: printf("\x1b[48;2;0;0;0m"); break;
                case 0x01: printf("\x1b[48;2;100;196;255m"); break;     // 青のインベーダーの色、シェルターの色
                case 0x02: printf("\x1b[48;2;170;223;8m"); break;  // 緑のインベーダーの色
                case 0x03: printf("\x1b[48;2;0;255;255m"); break;
                case 0x04: printf("\x1b[48;2;255;0;0m"); break;
                case 0x05: printf("\x1b[48;2;255;0;255m"); break;
                case 0x06: printf("\x1b[48;2;255;183;44m"); break;  // 黄のインベーダーの色
                case 0x07: printf("\x1b[48;2;255;255;255m"); break;

                case 0x08: printf("\x1b[48;2;0;0;0m"); break;
                case 0x09: printf("\x1b[48;2;126;192;255m"); break; // 残機の色
                case 0x0A: printf("\x1b[48;2;141;229;59m"); break;  // ゲームNo.(レベル)の色
                case 0x0B: printf("\x1b[48;2;74;232;139m"); break;  // 得点の色
                case 0x0C: printf("\x1b[48;2;255;0;0m"); break;
                case 0x0D: printf("\x1b[48;2;255;0;255m"); break;
                case 0x0E: printf("\x1b[48;2;255;255;0m"); break;
                case 0x0F: printf("\x1b[48;2;255;255;255m"); break;
            }
            // 1マス描画
            printf(" ");
        }
	    printf("\x1b[38;2;255;255;255m"); printf("\x1b[48;2;0;0;0m"); // 黒地に白戻しておく
        printf("\x1b[K\n"); // 行末までクリア
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
        printf("%02X:%02X %02X %02X %02X | ", i, m0, m1, m2, m3);
        i += 0x10;
        m0 = readMem((i << 2) | 0);
        m1 = readMem((i << 2) | 1);
        m2 = readMem((i << 2) | 2);
        m3 = readMem((i << 2) | 3);
        printf("%02X:%02X %02X %02X %02X\x1b[K\n", i, m0, m1, m2, m3);
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
    printf("HL:%02X:%X M:%02X STB:%X \x1b[K\n", H, L, M, STB);
    printf("A1:%02X A2:%02X A3:%02X A4:%02X \x1b[K\n", A1, A2, A3, A4);
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
