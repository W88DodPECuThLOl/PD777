#pragma once

#ifdef BUILD_WASM

// インポート(JavaScriptの関数)
#define WASM_IMPORT(MODULE,NAME) __attribute__((import_module(MODULE), import_name(NAME)))
// エクスポート(watの関数)
#define WASM_EXPORT              __attribute__((visibility("default")))

#else

// インポート(JavaScriptの関数)
#define WASM_IMPORT(MODULE,NAME)
// エクスポート(watの関数)
#define WASM_EXPORT

#endif // BUILD_WASM
