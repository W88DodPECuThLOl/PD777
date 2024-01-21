#include "catLowLevelDeviceAudioXAudio2.h"

namespace cat::core::snd {

LowLevelDeviceAudioXAudio2::LowLevelDeviceAudioXAudio2()
{
	// エンジンの作成
#if !defined(NDEBUG)
	HRESULT hr = XAudio2Create( &m_XAudio2, XAUDIO2_DEBUG_ENGINE ); // デバッグビルドの場合は、デバッグ版エンジンを使用
#else
	HRESULT hr = XAudio2Create( &m_XAudio2, 0 );
#endif
	if(FAILED(hr)) [[unlikely]] {
		return; // 作成に失敗した
	}

	// マスターボイスの作成
	IXAudio2MasteringVoice* masterVoice;
	hr = m_XAudio2->CreateMasteringVoice( &masterVoice );
	if(FAILED(hr)) [[unlikely]] {
		return; // 作成に失敗した
	}
	m_masterVoice.reset( masterVoice );

	// グループ用のサブミックスボイスを作成
	UINT32 InputChannels[1] = {1};
	UINT32 InputSampleRate[1] = {48000};
	for(std::uint32_t i = 0; i < 1; i++) {
		IXAudio2SubmixVoice* submixVoice;
		hr = m_XAudio2->CreateSubmixVoice(&submixVoice,InputChannels[i],InputSampleRate[i],0,0,0,0);
		m_submixVoice.emplace_back( submixVoice );
	}
}

HRESULT
LowLevelDeviceAudioXAudio2::CreateSourceVoice( IXAudio2SourceVoice** ppSourceVoice, const WAVEFORMATEX* pSourceFormat, UINT32 Flags, float MaxFrequencyRatio, IXAudio2VoiceCallback* pCallback, const XAUDIO2_VOICE_SENDS* pSendList, const XAUDIO2_EFFECT_CHAIN* pEffectChain )
{
	return m_XAudio2->CreateSourceVoice( ppSourceVoice, pSourceFormat, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain );
}

AudioHandle
LowLevelDeviceAudioXAudio2::play( decoder::AudioDecoder* decoder, const AudioGroup group )
{
	auto soundSouece = new X2AudioSoundSource();
	soundSouece->create( this, decoder, group );
	soundSouece->play();
	//m_soundSourcePool.emplace_back( soundSouece );
	return reinterpret_cast<AudioHandle>( soundSouece );
}

bool
LowLevelDeviceAudioXAudio2::stop( AudioHandle audioHandle )
{
	return ((X2AudioSoundSource*)audioHandle)->stop();
}

} // namespace cat::core::snd








namespace cat::core::snd {

X2AudioSoundSource::X2AudioSoundSource()
	: m_sourceBufferManager( std::make_unique<SourceBufferManager<XAUDIO2_BUFFER>>() )
	, m_decoder()
	, m_decoderThread( nullptr )
	, m_handleEventPause(0)
{
}

AudioHandle
X2AudioSoundSource::create(LowLevelDeviceAudioXAudio2* audioDevice, decoder::AudioDecoder* decoder, const AudioGroup group)
{
	m_decoder.reset( decoder );
	m_decoder->initialize(0,0);

	WAVEFORMATEX wfx {};
	wfx.wFormatTag = WAVE_FORMAT_PCM;   /* format type */
	wfx.nChannels = m_decoder->getChannels();   /* number of channels (i.e. mono, stereo...) */
	wfx.nSamplesPerSec = m_decoder->getSamplesPerSec();     /* sample rate */
	wfx.wBitsPerSample = m_decoder->getBitsPerSample();    /* number of bits per sample of mono data */
	wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;        /* block size of data */
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;    /* for buffer estimation */
	wfx.cbSize = 0;             /* the count in bytes of the size of */
									/* extra information (after cbSize) */

	// バッファを2つ作成
	for(int i = 0; i < 2; ++i) {
		XAUDIO2_BUFFER x2buffer {};
		x2buffer.pAudioData = new BYTE[wfx.nAvgBytesPerSec];
		x2buffer.pContext = this;
		m_sourceBufferManager->push_back( x2buffer );
	}

	// ボイスを作成
	XAUDIO2_SEND_DESCRIPTOR send = { 0, audioDevice->getSubmixVoice( group ) };
	XAUDIO2_VOICE_SENDS sendList = { 1, &send };
	IXAudio2SourceVoice* sourceVoice = nullptr;
	HRESULT hr = audioDevice->CreateSourceVoice( &sourceVoice, &wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &m_voiceCallback, &sendList, NULL );
	m_sourceVoice.reset(sourceVoice);

	// デコードスレッド起動
	m_handleEventPause = CreateEvent( nullptr, true, true, TEXT("pause") );
	m_decoderThread = std::make_unique<std::thread>( std::thread( [this, wfx](){
		this->decoderThreadMain( wfx.nAvgBytesPerSec / 30 );
	} ));

	return (AudioHandle)this;
}

bool
X2AudioSoundSource::play()
{
	// デコードスレッド開始と再生開始
	SetEvent( m_handleEventPause );

	return true;
}

bool
X2AudioSoundSource::stop()
{
	if(!m_sourceVoice) [[unlikely]] {
		return false;
	}

	// ボイスを停止
	m_sourceVoice->Stop();
	// デコーダースレッドを停止
	ResetEvent( m_handleEventPause );
	// 溜まっているボイスをフラッシュ
	m_sourceVoice->FlushSourceBuffers();

	return true;
}

void
X2AudioSoundSource::decoderThreadMain( int decocdSize )
{
	// 最初の1個をデコードして
	bool decodeFinish = m_sourceBufferManager->writeBuffer( [&]( XAUDIO2_BUFFER* x2buffer ) {
		x2buffer->AudioBytes = m_decoder->decode( (void*)x2buffer->pAudioData, decocdSize );
		x2buffer->Flags = x2buffer->AudioBytes ? 0 : XAUDIO2_END_OF_STREAM;
		return x2buffer->AudioBytes == 0;
	} );
	// ソースに書き込んでおく
	if(auto x2buffer = m_sourceBufferManager->lockReadBuffer(); x2buffer) {
		m_sourceVoice->SubmitSourceBuffer( x2buffer );
	} else {
		return; // エラー
	}

	// 再生開始待ち
	if(auto rc = WaitForSingleObject( m_handleEventPause, INFINITE ); rc == WAIT_OBJECT_0) [[likely]] {
		m_sourceVoice->Start();
	} else if(rc == WAIT_ABANDONED_0) [[unlikely]] {
		return; // 終了
	}

	while(!decodeFinish) {
		decodeFinish = m_sourceBufferManager->writeBuffer([this, decocdSize](XAUDIO2_BUFFER* x2buffer){
			x2buffer->AudioBytes = m_decoder->decode( (void*)x2buffer->pAudioData, decocdSize );
			x2buffer->Flags = x2buffer->AudioBytes ? 0 : XAUDIO2_END_OF_STREAM;
			return x2buffer->AudioBytes == 0;
		});
	}
	return;
}

} // namespace cat::core::snd
