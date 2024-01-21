#include "AudioXAudio2Callback.h"
#include "catLowLevelDeviceAudioXAudio2.h"

namespace cat::core::snd::XAudio2 {

void
VoiceCallback::OnBufferEnd(void * pBufferContext)
{
	X2AudioSoundSource* ss = (X2AudioSoundSource*)pBufferContext;

	// 使い終わったバッファを解放
	ss->m_sourceBufferManager->unlockReadBuffer();
	// 新しくバッファを積む
	if(auto x2buffer = ss->m_sourceBufferManager->lockReadBuffer(); x2buffer) {
		ss->m_sourceVoice->SubmitSourceBuffer( x2buffer );
	}
}

} // namespace cat::core::XAudio2