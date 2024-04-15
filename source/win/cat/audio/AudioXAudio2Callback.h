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
	STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired) override {}

	// ボイスの終了のプロセッシングパスの直後に呼び出される
    STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) override {}

	// ボイスが連続オーディオストリームの再生を終了したときに呼び出される
    STDMETHOD_(void, OnStreamEnd) (THIS) override {}

	// ボイスが新しいオーディオバッファーの処理を開始しようとするときに呼び出される
    STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) override {}

	// ボイスがバッファーの処理を終了したときに呼び出される
    STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext) override;

	// ボイスがループの最後に到達したときに呼び出される
    STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) override {}

	// ボイスの処理中に重大なエラーが発生したときに呼び出される
    STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) override {}
};

} // namespace cat::core::snd::XAudio2
