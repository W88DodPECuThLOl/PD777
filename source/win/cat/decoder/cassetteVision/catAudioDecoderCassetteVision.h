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
	double currentR = 0;
	double currentL = 0;
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
					if(s.counter-- <= 0) {
						src.score.pop();
					}
				}
				src.mtxScore.unlock();
			}

			{
				auto stepL = source[0].freq *(2.0*3.141592653) / getSamplesPerSec();
				auto stepR = source[1].freq *(2.0*3.141592653) / getSamplesPerSec();
				auto r = 0.0;
				auto l = 0.0;
				if(source[0].freq > 1) {
					// l = std::sin(currentL);
					l = currentL > 3.141592653 ? -1 : 1;
					currentL = std::fmod(currentL + stepL, 2.0 * 3.141592653);
				}
				if(source[1].freq > 1) {
					//r = std::sin(currentR);
					r = currentR > 3.141592653 ? -1 : 1;
					currentR = std::fmod(currentR + stepR, 2.0 * 3.141592653);
				}

				*buf++ = (r + l) * 0.5 * 32767.0 * 0.03;
				size -= 2;
			}
		}
		return readSize;
	}
public:
	virtual std::int32_t getChannels() const override { return 1; }
	virtual std::int32_t getSamplesPerSec() const override { return 48000; }
	virtual std::int32_t getBitsPerSample() const override { return 16; }

	struct Score {
		std::int64_t counter;
		std::uint8_t value;
	};
	struct SourceInfo {
		std::int64_t prevClockCounter = 0;
		std::mutex	mtxScore;
		std::queue<Score> score;
		double currentR = 0;
		double currentL = 0;
		std::int32_t freq = 0;
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

	void setScore(const std::int64_t clockCounter, const std::uint8_t value, const std::int32_t index)
	{
		auto& src = source[index];

		Score s;
		s.counter = (clockCounter - src.prevClockCounter) * CPUClock_period_s * 48000.0;
		s.counter *= 0.98; // 少し早くしておく
		if(s.counter <= 48000.0 * (1.0/60.0)) {
			s.value = value;
			src.mtxScore.lock();
			if(src.score.size() < 128) {
				src.score.push(s);
			}
			src.mtxScore.unlock();
		}
		src.prevClockCounter = clockCounter;
	}

	void setFLS(const std::int64_t clockCounter, const std::uint8_t value)
	{
		setScore(clockCounter, value, 0);
	}
	void setFRS(const std::int64_t clockCounter, const std::uint8_t value)
	{
		setScore(clockCounter, value, 1);
	}
};

} // namespace cat::core::decoder