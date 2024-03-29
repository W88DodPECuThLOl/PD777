#pragma once

#include "../../../core/catLowBasicTypes.h"
#include "../decoder/catAudioDecoder.h"
#include "AudioXAudio2Callback.h"
#include "catLowLevelDeviceAudioSourceBufferManager.h"

#if defined(_WIN32)
#define _WIN32_DCOM   // CoInitalizeEx関数使用を宣言
#include <xaudio2.h>
#include <wrl/client.h>
#endif // defined(_WIN32)
#include <vector>
#include <memory>
#include <thread>

namespace std {
	template<>
	struct default_delete<IXAudio2MasteringVoice> {
		void operator()( IXAudio2MasteringVoice* voice ) const { voice->DestroyVoice(); }
	};
};
namespace std {
	template<>
	struct default_delete<IXAudio2SubmixVoice> {
		void operator()( IXAudio2SubmixVoice* voice ) const { voice->DestroyVoice(); }
	};
};
namespace std {
	template<>
	struct default_delete<IXAudio2SourceVoice> {
		void operator()( IXAudio2SourceVoice* voice ) const { voice->Stop(); voice->DestroyVoice(); }
	};
};

namespace cat::core::snd {

using AudioGroup = s32;
using AudioHandle = void*;

/**
 * @brief	低レベルなオーディオ(XAudio2)
 */
class LowLevelDeviceAudioXAudio2 /*  : public LowLevelDeviceAudio */ {
	Microsoft::WRL::ComPtr<IXAudio2> m_XAudio2;
	std::unique_ptr<IXAudio2MasteringVoice> m_masterVoice;
	std::vector<std::unique_ptr<IXAudio2SubmixVoice>> m_submixVoice;
	std::unique_ptr<IXAudio2SourceVoice> m_sourceVoice;
public:

	/**
	 * @brief	コンストラクタ
	 */
	LowLevelDeviceAudioXAudio2();

	/**
	 * @brief	再生する
	 * @param[in]		group	どのグループで再生するか
	 * @param[in,out]	decoder	デコーダ
	 */
	virtual AudioHandle play( decoder::AudioDecoder* decoder, const AudioGroup group );

	/**
	 * 停止する
	 */
	virtual bool stop( AudioHandle audioHandle );

public:
	IXAudio2SubmixVoice* getSubmixVoice( const AudioGroup group ) { return m_submixVoice[group].get(); }
	HRESULT CreateSourceVoice( IXAudio2SourceVoice** ppSourceVoice,
                                  const WAVEFORMATEX* pSourceFormat,
                                  UINT32 Flags = 0,
                                  float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO,
                                  IXAudio2VoiceCallback* pCallback = NULL,
                                  const XAUDIO2_VOICE_SENDS* pSendList = NULL,
                                  const XAUDIO2_EFFECT_CHAIN* pEffectChain = NULL );
};

} // namespace cat::core::snd

namespace cat::core::snd {

class LowLevelDeviceAudioXAudio2;

class X2AudioSoundSource {
public:
	std::unique_ptr<SourceBufferManager<XAUDIO2_BUFFER>> m_sourceBufferManager;
	std::unique_ptr<decoder::AudioDecoder> m_decoder;
	std::unique_ptr<std::thread> m_decoderThread;

	std::unique_ptr<IXAudio2SourceVoice> m_sourceVoice;
	XAudio2::VoiceCallback m_voiceCallback;
public:
	X2AudioSoundSource();
	virtual ~X2AudioSoundSource() {}

public:
	virtual AudioHandle create(LowLevelDeviceAudioXAudio2* audioDevice, decoder::AudioDecoder* decoder, const AudioGroup group);

	virtual bool play();
	virtual bool stop();

	decoder::AudioDecoder* getDecoder() { return m_decoder.get(); }

private:
	HANDLE m_handleEventPause;
	void decoderThreadMain( int decocdSize );
};

} // namespace cat::core::snd
