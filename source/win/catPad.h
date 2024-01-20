#pragma once

#include "../core/catLowBasicTypes.h"

namespace cat::core::pad {

using GamePadButtonState = u64;

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
	f32	value;
};

/**
 * @brief	アナログスティックの状態
 */
struct GamePadAnalogState {
	f32	x;
	f32	y;
};

//! ゲームパッドの状態
struct GamePadState {
	/**
	 * @brief	トリガー(1次元)の状態数
	 */
	static constexpr size_t GAME_PAD_TRIGGER_MAX_COUNT = 4;
	/**
	 * @brief	アナログスティック(2次元)の状態数
	 */
	static constexpr size_t GAME_PAD_ANALOG_MAX_COUNT = 4;

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
	f32 leftMoterSpeed;
	/**
	 * @brief	右側の振動パラメータ(0.0f～1.0f)
	 */
	f32 rightMoterSpeed;
};

inline void
clear( GamePadState* pGamePadState )
{
	pGamePadState->buttons = 0;
	for(auto& trigger : pGamePadState->triggers) { trigger.value = 0; }
	for(auto& analog : pGamePadState->analogs) { analog.y = analog.x = 0; }
}

void
getPadState(s32 gamePadIndex, GamePadState* pGamePadState);

} // namespace cat::core::pad
