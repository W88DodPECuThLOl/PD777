#pragma once

// #include "catLowBasicTypes.h"

/**
 * @brief メモリを確保する
 * 
 * @param[in]	size	確保するメモリサイズ
 * @return	確保したメモリ
 */
void* catLowMemAlloc(size_t size);

/**
 * @brief メモリを解放する
 * 
 * @param[in]	ptr	解放するメモリ
 */
void catLowMemFree(void* ptr);

/**
 * @brief メモリを確保する
 * 
 * @param[in]	size	確保するメモリサイズ
 * @return	確保したメモリ
 */
[[nodiscard]] void* operator new(size_t size);

/**
 * @brief メモリを解放する
 * 
 * @param[in]	ptr	解放するメモリ
 */
void operator delete(void* ptr) noexcept;

/**
 * @brief 配列用のメモリを確保する
 * 
 * @param[in]	size	確保するメモリサイズ
 * @return	確保したメモリ
 */
[[nodiscard]] void* operator new[](size_t size);

/**
 * @brief 配列用のメモリを解放する
 * 
 * @param[in]	ptr	解放するメモリ
 */
void operator delete[](void*ptr) noexcept;

/**
 * @brief 配置new
 * 
 * @param[in]	ptr		newに使うポインタ
 * @param[in]	size	ポインタのサイズ
 * @return	ptrを返す
 */
[[nodiscard]] void* operator new(size_t size, void* ptr) noexcept;
