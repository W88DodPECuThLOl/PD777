#if BUILD_WASM

#include "../core/catLowBasicTypes.h"

// Wasmサポート

// vtables.
extern "C" {
  const void *_ZTVN10__cxxabiv123__fundamental_type_infoE;
  const void *_ZTVN10__cxxabiv117__class_type_infoE;
  const void *_ZTVN10__cxxabiv120__si_class_type_infoE;
  const void *_ZTVN10__cxxabiv121__vmi_class_type_infoE;
  const void *_ZTVN10__cxxabiv119__pointer_type_infoE;
  const void *_ZTVN10__cxxabiv129__pointer_to_member_type_infoE;
};

// メモ)clangちゃんが、勝手にmemcpy使ってしまうので
extern "C"
void* memcpy(void* dest, const void* src, const size_t size)
{
	const u8* s = (const u8*)src;
	u8* d = (u8*)dest;
	u8* e = d + size;
	while(d != e) { *d++ = *s++; }
	return dest;
}

// メモ)clangちゃんが、勝手にmemset使ってしまうので
extern "C"
void* memset(void* dest, int c, size_t size)
{
	u8* d = (u8*)dest;
	u8* e = d + size;
	while(d != e) { *d++ = c; }
	return dest;
}

#endif // BUILD_WASM
