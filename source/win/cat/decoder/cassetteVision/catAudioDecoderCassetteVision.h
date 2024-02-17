#pragma once

#include "../../../../core/catLowBasicTypes.h"
#include "../catAudioDecoder.h"
#include <cstring>
#include <vector>
#include <queue>
#include <cmath>
#include <mutex>

namespace cat::core::decoder {

class CassetteVisionAudioDecoder final : public AudioDecoder {

	/**
	 * @brief 残響効果の開始時間(秒)
	 */
	static constexpr double REV_EFFECT_NOTE_OFF_TIME = 0.04;

	/**
	 * @brief 残響効果の減衰の係数（1に近い程、余韻が長くなる）
	 */
	static constexpr double REV_EFFECT_ATTENUATION = 0.9994;

public:
	CassetteVisionAudioDecoder() {}
	virtual ~CassetteVisionAudioDecoder() {}
public:
	static bool canDecodeThisFile( const char* filename ) { return true; }
	static bool canDecodeThisFile( const void* data, [[maybe_unused]] const size_t dataSize ) { return true; }
	virtual bool initialize( const char* filename ) override { return true; }
	virtual bool initialize( const void* data, const size_t dataSize ) override { return true; }
	virtual void terminate() override {}

	std::int64_t clockCounter = 0;
	virtual std::size_t decode( void* buffer, const std::size_t bufferSize ) override {
		std::memset( buffer, 0, bufferSize );
		auto readSize = bufferSize;

		std::int16_t* buf = (std::int16_t*)buffer;
		auto size = bufferSize;

		while(size > 0) {
			for(auto& src : source) {
				src.mtxScore.lock();
				while(!src.score.empty()) {
					Score s = src.score.front();
					if(s.counter < 0) {
						src.score.pop();
					} else {
						break;
					}
				}
				if(!src.score.empty()) {
					Score& s = src.score.front();
					src.freq = convert(s.value);
					if(s.noteOn) {
						src.rev = s.rev;
						src.revTime = 0;
						src.revVolume = 1;
					}
					if(s.counter-- <= 0) {
						src.score.pop();
					}
				}
				src.mtxScore.unlock();
			}

			double output = 0.0;
			for(auto& src : source) {
				if(src.freq > 1) {
					auto v = src.phase > 3.141592653 ? -1.0 : 1.0;
					const auto step = src.freq *(2.0*3.141592653) / getSamplesPerSec();
					src.phase = std::fmod(src.phase + step, 2.0 * 3.141592653);
					// REVの処理
					if(src.rev) {
						v *= src.revVolume;
						if(src.revTime++ > getSamplesPerSec() * REV_EFFECT_NOTE_OFF_TIME) {
							src.revVolume *= REV_EFFECT_ATTENUATION;
						}
					}
					// 合成
					output += v;
				}
			}
			*buf++ = output * 0.5 * 32767.0 * 0.02;
			size -= 2;
		}
		return readSize;
	}
public:
	virtual std::int32_t getChannels() const override { return 1; }
	virtual std::int32_t getSamplesPerSec() const override { return 48000; }
	virtual std::int32_t getBitsPerSample() const override { return 16; }

	struct Score {
		std::int64_t counter;
		/**
		 * @brief FLSレジスタ、FRSレジスタに書き込まれた時の値
		 */
		std::uint8_t value;
		/**
		 * @brief 残響効果が有効かどうか
		 */
		bool rev;
		/**
		 * @brief ノートオンで発生したものかどうか
		 * @note  FLSレジスタ、FRSレジスタに書き込まれて発生したものならtrue。
		 *        定期的な更新で発生したものならfalse。
		 */
		bool noteOn;
	};
	struct SourceInfo {
		std::int64_t prevClockCounter = 0;
		std::mutex	mtxScore;
		std::queue<Score> score;
		double phase = 0.0;
		std::int32_t freq = 0;

		// REV EFFECT
		bool rev = false;
		double revVolume = 0.0;
		std::int64_t revTime;
	};
	SourceInfo source[2];

	double convert(const std::uint8_t value) {
		/*
		A counter which initial value is FLS[7:1] or FRS[7:1] down-counts every horizontal period (15.734 KHz).
		Two sets of sound down-counters are implemented and the sounds generated are mixed together. Therefore,
		SOUND pin outputs monaural sound superimposed the two sound sources.
		*/
		if(value >= 2) {
			return 15734.0 / (value - 1);
		} else {
			return 0;
		}
	}
public:
	static constexpr auto Clock_Hz = 3.579545 * 1000.0 * 1000.0;
	static constexpr auto CPUClock_Hz = Clock_Hz / 2.5;
	static constexpr auto CPUClock_period_s = 1.0 / CPUClock_Hz;

	void resetInnerClock(const std::int64_t clockCounter) {
		source[0].prevClockCounter = clockCounter;
		source[1].prevClockCounter = clockCounter;
	}

	void setScore(const std::int64_t clockCounter, const std::uint8_t value, const std::int32_t index, bool noteOn, bool reverberatedSoundEffect = false)
	{
		auto& src = source[index];

		Score s;
		s.counter = (clockCounter - src.prevClockCounter) * CPUClock_period_s * getSamplesPerSec();
		s.counter *= 0.98; // 少し早くしておく
		if(s.counter <= 48000.0 * (1.0/60.0)) {
			s.value  = value;
			s.rev    = reverberatedSoundEffect;
			s.noteOn = noteOn;
			src.mtxScore.lock();
			if(src.score.size() < 128) {
				src.score.push(s);
			}
			src.mtxScore.unlock();
		}
		src.prevClockCounter = clockCounter;
	}

	void updateFLS(const std::int64_t clockCounter, const std::uint8_t value)
	{
		setScore(clockCounter, value, 0, false);
	}
	void setFLS(const std::int64_t clockCounter, const std::uint8_t value, const bool reverberatedSoundEffect)
	{
		setScore(clockCounter, value, 0, true, reverberatedSoundEffect);
	}
	void updateFRS(const std::int64_t clockCounter, const std::uint8_t value)
	{
		setScore(clockCounter, value, 1, false);
	}
	void setFRS(const std::int64_t clockCounter, const std::uint8_t value, const bool reverberatedSoundEffect)
	{
		setScore(clockCounter, value, 1, true, reverberatedSoundEffect);
	}
};

} // namespace cat::core::decoder