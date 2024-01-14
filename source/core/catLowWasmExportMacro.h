#pragma once

#if defined(BUILD_WASM)

// WebAssembly

// インポート(JavaScriptの関数)
#define WASM_IMPORT(MODULE,NAME) __attribute__((import_module(MODULE), import_name(NAME)))
// エクスポート(watの関数)
#define WASM_EXPORT              __attribute__((visibility("default")))


#else

// VC++

// インポート(JavaScriptの関数)
#define WASM_IMPORT(MODULE,NAME)
#define WASM_EXPORT

#endif
