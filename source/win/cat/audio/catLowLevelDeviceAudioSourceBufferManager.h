#pragma once

#include "../../../core/catLowBasicTypes.h"

#include <cstdint>
#include <cstddef>
#include <vector>

#include <mutex>
#include <condition_variable>
#include <functional>

namespace cat::core::snd {

template<typename T>
class SourceBufferManager {
	/**
	 * @brief	書き込み位置
	 */
	volatile uint8_t m_writePosition;

	/**
	 * @brief	読み込み位置
	 */
	volatile uint8_t m_readPosition;

	/**
	 * @brief	バッファ
	 */
	std::vector<T> m_buffers;

	//
	// 排他制御用
	//

	/**
	 * @brief	バッファロック用のミューテックス
	 */
	std::mutex	m_mtxBuffer;

	/*
	 * @brief	バッファが満杯ではなくなった条件変数
	 */
	std::condition_variable	m_cvBufferIsNotFull;

	/*
	 * @brief	バッファが空ではなくなった条件変数
	 */
	std::condition_variable	m_cvBufferIsNotEmpty;

public:
	/*
	 * @brief	バッファの型
	 */
	using BufferType = T;

public:
	/*
	 * @brief	コンストラクタ
	 */
	SourceBufferManager()
		: m_writePosition( 0 )
		, m_readPosition( 0 )
	{
	}

	/*
	 * @brief	バッファを準備する
	 * @param[in]	buffer	バッファ
	 */
	void push_back( T& buffer )
	{
		m_buffers.push_back( buffer );
	}

	/**
	 * @brief	バッファ書き込む
	 * @param[in]	writeFunction	書き込み用の関数
	 * @return	@a writeFunction の戻り値
	 */
	bool writeBuffer( std::function<bool(T*)> writeFunction ) {
		std::unique_lock<std::mutex> lock(m_mtxBuffer);
		m_cvBufferIsNotFull.wait(lock, [&](){ return !full(); });
		const bool needNotify = empty();

		const auto rc = writeFunction( &m_buffers[m_writePosition] );

		m_writePosition = (m_writePosition + 1) % m_buffers.size();
		if(needNotify) {
			m_cvBufferIsNotEmpty.notify_all(); // 空から１つバッファを追加して、空では無くなったので通知する
		}
		return rc;
	}

	/**
	 * @brief	バッファ読み込み
	 * @param[in]	readFunction	読み込み用の関数
	 * @return	@a readFunction の戻り値
	 */
	bool readBuffer(std::function<bool(const T*)> readFunction) {
		std::unique_lock<std::mutex> lock(m_mtxBuffer);
		m_cvBufferIsNotEmpty.wait(lock, [&](){ return !empty(); });
		const auto needNotify = full();

		const auto rc = readFunction( &m_buffers[m_readPosition] );

		m_readPosition = (m_readPosition + 1) % m_buffers.size();
		if(needNotify) {
			m_cvBufferIsNotFull.notify_all(); // 満杯から１つバッファを消費して、空きができたので通知する
		}
		return rc;
	}

	const T* lockReadBuffer() {
		std::unique_lock<std::mutex> lock(m_mtxBuffer);
		m_cvBufferIsNotEmpty.wait(lock, [&](){ return !empty(); });
		return &m_buffers[m_readPosition];
	}

	bool unlockReadBuffer() {
		std::unique_lock<std::mutex> lock(m_mtxBuffer);
		m_cvBufferIsNotEmpty.wait(lock, [&](){ return !empty(); });
		const auto needNotify = full();

		m_readPosition = (m_readPosition + 1) % m_buffers.size();

		if(needNotify) {
			m_cvBufferIsNotFull.notify_all(); // 満杯から１つバッファを消費して、空きができたので通知する
		}
		return true;
	}

private:
	/**
	 * @brief	バッファが空かどうか
	 * @return	バッファが空のとき true を、そうでないときは false を返す。
	 * @retval	true  : バッファが空
	 * @retval	false : バッファが空ではない
	 */
	inline bool empty() const noexcept { return m_writePosition == m_readPosition; }

	/**
	 * @brief	バッファが満杯かどうか
	 * @return	バッファが満杯のとき true を、そうでないときは false を返す。
	 * @retval	true  : バッファが満杯
	 * @retval	false : バッファが満杯ではない
	 */
	inline bool full() const noexcept { return ((m_writePosition + 1) % m_buffers.size()) == m_readPosition; }
};

} // namespace cat::core::snd