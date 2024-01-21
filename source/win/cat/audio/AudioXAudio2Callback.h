#pragma once

#include "../../../core/catLowBasicTypes.h"

#if defined(_WIN32)
#define _WIN32_DCOM   // CoInitalizeEx関数使用を宣言
#include <xaudio2.h>
#include <wrl/client.h>
#endif // defined(_WIN32)

namespace cat::core::snd::XAudio2 {

class VoiceCallback : public IXAudio2VoiceCallback
{
public:
	VoiceCallback() {}
	virtual ~VoiceCallback() {}

	// ボイス開始のプロセッシングパスの直前に呼び出される
	void OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}

	// ボイスの終了のプロセッシングパスの直後に呼び出される
	void OnVoiceProcessingPassEnd() {}

	// ボイスが連続オーディオストリームの再生を終了したときに呼び出される
	void OnStreamEnd() {}

	// ボイスが新しいオーディオバッファーの処理を開始しようとするときに呼び出される
	void OnBufferStart(void * pBufferContext) {}

	// ボイスがバッファーの処理を終了したときに呼び出される
	void OnBufferEnd(void * pBufferContext);

	// ボイスがループの最後に到達したときに呼び出される
	void OnLoopEnd(void * pBufferContext) {}

	// ボイスの処理中に重大なエラーが発生したときに呼び出される
	void OnVoiceError(void * pBufferContext, HRESULT Error) {}
};

} // namespace cat::core::snd::XAudio2
