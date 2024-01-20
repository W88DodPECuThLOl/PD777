#include "WinPD777.h"

#include <Windows.h>
#include <Xinput.h>
#pragma comment (lib, "xinput.lib")
#include <string>
#include <cmath>

namespace {

using GamePadButtonState = std::uint64_t;

namespace ButtonMask {

/**
 * @brief	ゲームパッドのデジタルボタンの状態
 */
enum GamePadButtonMask : GamePadButtonState {
	/**
	 * @brief	デジタル入力の上ボタン
	 */
	DPAD_UP = 0x0001,
	/**
	 * @brief	デジタル入力の下ボタン
	 */
	DPAD_DOWN = 0x0002,
	/**
	 * @brief	デジタル入力の左ボタン
	 */
	DPAD_LEFT = 0x0004,
	/**
	 * @brief	デジタル入力の右ボタン
	 */
	DPAD_RIGHT = 0x0008,
	/**
	 * @brief	スタートボタン
	 */
	START = 0x0010,
	/**
	 * @brief	バックボタン
	 */
	BACK = 0x0020,
	LEFT_THUMB = 0x0040,
	RIGHT_THUMB = 0x0080,
	LEFT_SHOULDER = 0x0100,
	RIGHT_SHOULDER = 0x0200,
	/**
	 * @brief	Aボタン
	 */
	A = 0x1000,
	/**
	 * @brief	Bボタン
	 */
	B = 0x2000,
	/**
	 * @brief	Xボタン
	 */
	X = 0x4000,
	/**
	 * @brief	Yボタン
	 */
	Y = 0x8000,

	/**
	 * @brief	コントローラーが接続されているかどうか
	 */
	CONTROLLER_IS_CONNECTED = 0x8000'0000'0000'0000
};

} //namespace ButtonMask

/**
 * @brief	トリガーの状態
 */
struct GamePadTriggerState {
	float	value;
};

/**
 * @brief	アナログスティックの状態
 */
struct GamePadAnalogState {
	float	x;
	float	y;
};

//! ゲームパッドの状態
struct GamePadState {
	/**
	 * @brief	トリガー(1次元)の状態数
	 */
	static constexpr std::size_t GAME_PAD_TRIGGER_MAX_COUNT = 4;
	/**
	 * @brief	アナログスティック(2次元)の状態数
	 */
	static constexpr std::size_t GAME_PAD_ANALOG_MAX_COUNT = 4;

	/**
	 * @brief	デジタルボタンの状態
	 */
	GamePadButtonState	buttons;
	/**
	 * @brief	トリガー(1次元)の状態
	 */
	GamePadTriggerState	triggers[GAME_PAD_TRIGGER_MAX_COUNT];
	/**
	 * @brief	アナログスティック(2次元)の状態
	 */
	GamePadAnalogState	analogs[GAME_PAD_ANALOG_MAX_COUNT];

	/**
	 * @brief	コントローラーが接続されているかどうか
	 * @return	コントローラーが接続されているかどうか
	 * @retval	true  : 接続されている
	 * @retval	false : 接続されていない
	 */
	bool isControllerConnected() const { return (buttons & ButtonMask::CONTROLLER_IS_CONNECTED) != 0; }
};

static_assert(GamePadState::GAME_PAD_TRIGGER_MAX_COUNT >= 2);
static_assert(GamePadState::GAME_PAD_ANALOG_MAX_COUNT >= 2);

/**
 * @brief	振動パラメータ
 */
struct GamePadVibration {
	/**
	 * @brief	左側の振動パラメータ(0.0f～1.0f)
	 */
	float leftMoterSpeed;
	/**
	 * @brief	右側の振動パラメータ(0.0f～1.0f)
	 */
	float rightMoterSpeed;
};

inline void
clear( GamePadState* pGamePadState )
{
	pGamePadState->buttons = 0;
	for(auto& trigger : pGamePadState->triggers) { trigger.value = 0; }
	for(auto& analog : pGamePadState->analogs) { analog.y = analog.x = 0; }
}

template<int THRESHOLD, int MAX_VALUE>
inline float
triggerAdjust( std::int32_t rawValue ) {
	if(auto v = rawValue - THRESHOLD; v <= 0) {
		return 0.0f;
	} else {
		return static_cast<float>( v ) / static_cast<float>(MAX_VALUE - THRESHOLD);
	}
}

inline void
analogAdjust(float& x, float& y, float rawX, float rawY, const float deadZone, const float maxValue)
{
	//determine how far the controller is pushed
	auto magnitude = std::sqrt( rawX*rawX + rawY*rawY);
	if(magnitude <= deadZone) {
		x = 0.0f;
		y = 0.0f;
		return;
	}
	const auto normalizedX = rawX / magnitude;
	const auto normalizedY = rawY / magnitude;

	const auto a = (maxValue - deadZone);
	magnitude -= deadZone;
	if(magnitude > a) {
		magnitude = a;
	}
	x = normalizedX * magnitude / a;
	y = normalizedY * magnitude / a;
}

void
getPadState(int gamePadIndex, GamePadState* pGamePadState)
{
	XINPUT_STATE state;
	if(SUCCEEDED(XInputGetState( gamePadIndex, &state ))) {
		// 接続されている

		// デジタルボタン
		pGamePadState->buttons = state.Gamepad.wButtons | ButtonMask::CONTROLLER_IS_CONNECTED;
		// トリガー
		static_assert(GamePadState::GAME_PAD_TRIGGER_MAX_COUNT >= 2);
		pGamePadState->triggers[0].value = triggerAdjust<XINPUT_GAMEPAD_TRIGGER_THRESHOLD,255>( state.Gamepad.bLeftTrigger );
		pGamePadState->triggers[1].value = triggerAdjust<XINPUT_GAMEPAD_TRIGGER_THRESHOLD,255>( state.Gamepad.bRightTrigger );
		for(auto i = 2; i < GamePadState::GAME_PAD_TRIGGER_MAX_COUNT; i++) {
			pGamePadState->triggers[i].value = 0;
		}
		// アナログ
		static_assert(GamePadState::GAME_PAD_ANALOG_MAX_COUNT >= 2);
		analogAdjust( pGamePadState->analogs[0].x, pGamePadState->analogs[0].y, state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, 32767 );
		analogAdjust( pGamePadState->analogs[1].x, pGamePadState->analogs[1].y, state.Gamepad.sThumbRX, state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, 32767 );
		for(auto i = 2; i < GamePadState::GAME_PAD_ANALOG_MAX_COUNT; i++) {
			pGamePadState->analogs[i].x = 0;
			pGamePadState->analogs[i].y = 0;
		}
	} else {
		// 接続されていない
		clear(pGamePadState);
	}
}

} // namespace


void
WinPD777::present()
{
    printf("\x1b[100A"); // 画面クリア
    for(int y = 0; y < 60; y++) {
        for(int x = 0; x < 90; x++) {
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
        printf("\x1b[K\n"); // 行末までクリア
    }
    printf("\x1b[38;2;255;255;255m"); printf("\x1b[48;2;0;0;0m"); // 黒地に白戻しておく
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
	int gamePadIndex = 0;
	GamePadState gamePadState;
	getPadState(gamePadIndex, &gamePadState);
	return (gamePadState.buttons & (ButtonMask::X | ButtonMask::DPAD_LEFT)) != 0;
}

bool
WinPD777::isPD2()
{
	int gamePadIndex = 0;
	GamePadState gamePadState;
	getPadState(gamePadIndex, &gamePadState);
	return (gamePadState.buttons & (ButtonMask::B | ButtonMask::DPAD_RIGHT)) != 0;
}

bool
WinPD777::isPD3()
{
	int gamePadIndex = 0;
	GamePadState gamePadState;
	getPadState(gamePadIndex, &gamePadState);
	return (gamePadState.buttons & (ButtonMask::Y | ButtonMask::DPAD_UP)) != 0;
}

bool
WinPD777::isPD4()
{
	int gamePadIndex = 0;
	GamePadState gamePadState;
	getPadState(gamePadIndex, &gamePadState);
	return (gamePadState.buttons & (ButtonMask::A | ButtonMask::DPAD_DOWN)) != 0;
}

u8
WinPD777::readKIN()
{
	int gamePadIndex = 0;
	GamePadState gamePadState;
	getPadState(gamePadIndex, &gamePadState);

	u8 value = (u8)KIN::None;
	if(gamePadState.buttons & (ButtonMask::START | ButtonMask::RIGHT_THUMB)) {
		value |= (u8)KIN::GameStartKey;
	}
	if(gamePadState.buttons & (ButtonMask::BACK | ButtonMask::LEFT_THUMB)) {
		value |= (u8)KIN::GameSelectKey;
	}
	if((gamePadState.buttons & ButtonMask::DPAD_LEFT) || (gamePadState.analogs[0].x < -0.3f)) {
		value |= (u8)KIN::LeverSwitchLeft;
	}
	if((gamePadState.buttons & ButtonMask::DPAD_RIGHT) || (gamePadState.analogs[0].x > 0.3f)) {
		value |= (u8)KIN::LeverSwitchRight;
	}
	if(gamePadState.buttons & (ButtonMask::A | ButtonMask::B | ButtonMask::X | ButtonMask::Y)) {
		value |= (u8)KIN::PushAll;
	}
	return value & 0x7F;
}
