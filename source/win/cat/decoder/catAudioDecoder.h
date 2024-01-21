/**
 * @file catAudioDecoder.h
 * @brief	音声デコード
 */
#pragma once

#include "../../../core/catLowBasicTypes.h"

#include <cstdint>
#include <cstddef>

namespace cat::core::decoder {

/**
 * @brief	音声デコードの基底クラス
 */
class AudioDecoder {
public:
	AudioDecoder() = default;
	virtual ~AudioDecoder() = default;
public:
	virtual bool initialize( const char* filename ) = 0;
	virtual bool initialize( const void* data, const size_t dataSize ) = 0;
	virtual void terminate() = 0;
	virtual std::size_t decode( void* buffer, const std::size_t bufferSize ) = 0;
	virtual std::int32_t getChannels() const = 0;
	virtual std::int32_t getSamplesPerSec() const = 0;
	virtual std::int32_t getBitsPerSample() const = 0;
};

} // namespace cat::core::decoder