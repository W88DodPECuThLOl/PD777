#pragma once

#include "../catAudioDecoder.h"
#include <cstring>

namespace cat::core::decoder {

class SilenceAudioDecoder final : public AudioDecoder {
public:
	SilenceAudioDecoder() {}
	virtual ~SilenceAudioDecoder() {}
public:
	[[maybe_unused]] static bool canDecodeThisFile( [[maybe_unused]] const char* filename ) { return true; }
	[[maybe_unused]] static bool canDecodeThisFile( [[maybe_unused]] const void* data, [[maybe_unused]] const size_t dataSize ) { return true; }
	virtual bool initialize( [[maybe_unused]] const char* filename ) override { return true; }
	virtual bool initialize( [[maybe_unused]] const void* data, [[maybe_unused]] const size_t dataSize ) override { return true; }
	virtual void terminate() override {}
	virtual std::size_t decode( void* buffer, const std::size_t bufferSize ) override { std::memset( buffer, 0, bufferSize ); return bufferSize; }
public:
	virtual std::int32_t getChannels() const override { return 2; }
	virtual std::int32_t getSamplesPerSec() const override { return 44100; }
	virtual std::int32_t getBitsPerSample() const override { return 16; }
};

} // namespace cat::core::decoder