#include "catPad.h"

#include <Windows.h>
#include <Xinput.h>
#pragma comment (lib, "xinput.lib")
#include <cmath>

namespace {

template<s32 THRESHOLD, s32 MAX_VALUE>
inline float
triggerAdjust( s32 rawValue ) {
	if(auto v = rawValue - THRESHOLD; v <= 0) {
		return 0.0f;
	} else {
		return static_cast<f32>( v ) / static_cast<f32>(MAX_VALUE - THRESHOLD);
	}
}

inline void
analogAdjust(f32& x, f32& y, f32 rawX, f32 rawY, const f32 deadZone, const f32 maxValue)
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

} // namespace

namespace cat::core::pad {

void
getPadState(s32 gamePadIndex, GamePadState* pGamePadState)
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

} // namespace cat::core::pad
