#include "catLowBasicTypes.h"
#include "catLowWasmExportMacro.h"

#define DEBUG_MEM_LEAK (0)

#if DEBUG_MEM_LEAK
namespace {
u64 alloc_counter = 0;
}
#endif // DEBUG_MEM_LEAK

/**
 * @brief 固定ブロック長でメモリを確保
 * 
 * @param	N			ブロック数
 * @param	BlockSize	１個のブロックのサイズ（バイト単位）
 */
template<u64 N, u64 BlockSize>
struct catLowBlockAllocator {
	/**
	 * @brief ブロック
	 */
	u8	block[N * BlockSize];

	/**
	 * @brief ブロックが使われているかどうか
	 */
#if DEBUG_MEM_LEAK
	u32  used[N] {0};
#else
	u8  used[N] {0};
#endif

	/**
	 * @brief ブロックを探す時の目安
	 */
	u64 prev = 0;
	/**
	 * @brief 満杯かどうか
	 */
	bool full = false;

	/**
	 * @brief １個のブロックのサイズ（バイト単位）を取得する
	 * 
	 * @return １個のブロックのサイズ（バイト単位）
	 */
	constexpr u64 getBlockSize() const noexcept { return BlockSize; }

	/**
	 * @brief メモリを確保する
	 * 
	 * @return	確保したメモリ
	 */
	void*
	alloc() noexcept
	{
		if(full) [[unlikely]] { return nullptr; }

		for(; prev < N; ++prev) {
			if(used[prev] == 0) {
#if DEBUG_MEM_LEAK
				used[prev] = alloc_counter;
#else
				used[prev] = 1;
#endif
				auto rc = (void*)(block + prev * BlockSize);
				prev = (prev + 1) % N;
				return rc;
			}
		}
		// 初めから検索する
		for(u64 i = 0; i < prev; ++i) {
			if(used[i] == 0) {
#if DEBUG_MEM_LEAK
				used[i] = alloc_counter;
#else
				used[i] = 1;
#endif
				prev = (i + 1) % N;
				return (void*)(block + i * BlockSize);
			}
		}
		// 空きがなかった
		full = true;
		return nullptr;
	}
	void unsafeFree(void* ptr) noexcept {
		const u64 index = ((u64)ptr - (u64)block) / BlockSize;
#if defined(catAssert)
		catAssert(used[index] != 0, "確保されていないブロックを解放しようとしました。");
#endif // defined(catAssert)
		if(used[index]) [[likely]] {
			used[index] = 0;
			prev = index;
			full = false;
		}
	}
	void free(void* ptr) noexcept {
		if(isMine(ptr)) {
			unsafeFree(ptr);
		}
	}
	bool isMine(void* ptr) const noexcept {
		return ((u64)&block[0] <= (u64)ptr) && ((u64)ptr < (u64)&block[N * BlockSize]);
	}

	u64 dump() const noexcept {
		u64 counter = 0;
		for(u64 i = 0; i < N; ++i) {
			if(used[i]) {
#if DEBUG_MEM_LEAK
				u8* ptr = (u8*)(block + i * BlockSize);
				printf("%d,%d ", (s32)i, (s32)used[i]);
				for(s32 k=0; k<BlockSize;k++) {
					printf("%02X", (u32)ptr[k]);
				}
				printf("\n");
#endif
				++counter;
			}
		}
		return counter;
	}
};


//using SmallMemoryBlock1 = catLowBlockAllocator<  256*1024, 32>;
//using SmallMemoryBlock2 = catLowBlockAllocator< 1024*1024, 64>;
//using SmallMemoryBlock3 = catLowBlockAllocator<   64*1024,128>;
//using SmallMemoryBlock4 = catLowBlockAllocator<   32*1024,256>;
//using SmallMemoryBlock5 = catLowBlockAllocator<   16*1024,512>;

//16MB版
using SmallMemoryBlock1 = catLowBlockAllocator<   16*1024, 32>;
using SmallMemoryBlock2 = catLowBlockAllocator<   32*1024, 64>;
using SmallMemoryBlock3 = catLowBlockAllocator<    4*1024,128>;
using SmallMemoryBlock4 = catLowBlockAllocator<    4*1024,256>;
using SmallMemoryBlock5 = catLowBlockAllocator<    4*1024,512>;

template<u64 BlockSize>
struct catLowAllocator {
	/**
	 * @brief ブロック
	 */
	u8	block[BlockSize];

	/**
	 * @brief ブロック属性
	 */
	enum class BLOCK_ATTR : u64 {
		/**
		 * @brief 空きブロック
		 */
		FREE_BLOCK = 0,
		/**
		 * @brief 使用中のブロック
		 */
		USED_BLOCK = 1,
	};

	/**
	 * @brief メモリブロックのヘッダ
	 */
	struct MemoryElement {
#if DEBUG_MEM_LEAK
		u32 guard;
		u32 signiture;
#endif // DEBUG_MEM_LEAK
		MemoryElement*	next;
		/**
		 * @brief ブロック全体のサイズ（ヘッダ部分含む）
		 */
		u64				size;
		BLOCK_ATTR		attr;
	};

	MemoryElement* getRoot() { return (MemoryElement*)block; }

	/**
	 * @brief メモリブロックを分割する
	 * 
	 * @param[in,out]	f		分割するメモリブロック
	 * @param[in]		size	分割するサイズ（ヘッダ部分を含まない）
	 * @return メモリブロック
	 */
	MemoryElement*
	splitBlock(MemoryElement* f, const u64 size)
	{
		// 新しく未使用のメモリブロック作成
		MemoryElement* u = (MemoryElement*)((u8*)f + sizeof(MemoryElement) + size); // 空きメモリブロックの後ろに作成
#if DEBUG_MEM_LEAK
		u->guard = 0xCCCCCCCC;
#endif
		u->next = f->next;
#if DEBUG_MEM_LEAK
		catAssert(u->next != u, "同じだめ");
#endif
		u->size = f->size - (sizeof(MemoryElement) + size);
		u->attr = BLOCK_ATTR::FREE_BLOCK;
#if DEBUG_MEM_LEAK
		catAssert(u->guard == 0xCCCCCCCC, "壊れてる");
#endif

		// 使用メモリブロックに変更
#if DEBUG_MEM_LEAK
		catAssert(f->guard == 0xCCCCCCCC, "壊れてる");
#endif
		f->next = u;
#if DEBUG_MEM_LEAK
		catAssert(f->next != f, "同じだめ");
#endif
		f->size = sizeof(MemoryElement) + size;
		f->attr = BLOCK_ATTR::USED_BLOCK;
		return f;
	}

	void
	fusionBlock(MemoryElement* u, MemoryElement* prevBlock)
	{
#if DEBUG_MEM_LEAK
		catAssert(u->guard == 0xCCCCCCCC, "壊れてる");
#endif
		u->attr = BLOCK_ATTR::FREE_BLOCK;

		// 次がfreeブロックならば、合体する
		if(auto* nextBlock = u->next; nextBlock) {
#if DEBUG_MEM_LEAK
			catAssert(nextBlock->guard == 0xCCCCCCCC, "壊れてる");
#endif
			if(nextBlock->attr == BLOCK_ATTR::FREE_BLOCK) {
				u->next = nextBlock->next;
#if DEBUG_MEM_LEAK
				catAssert(u->next != u, "同じだめ");
#endif
				u->size += nextBlock->size;
//				u->attr = BLOCK_ATTR::FREE_BLOCK;
			}
		}
		// ひとつ前がfreeブロックならば、合体する
		if(prevBlock && (prevBlock->attr == BLOCK_ATTR::FREE_BLOCK)) {
#if DEBUG_MEM_LEAK
			catAssert(prevBlock->guard == 0xCCCCCCCC, "壊れてる");
#endif
			prevBlock->next = u->next;
#if DEBUG_MEM_LEAK
			catAssert(prevBlock->next != prevBlock, "同じだめ");
#endif
			prevBlock->size += u->size;
			//prevBlock->attr = BLOCK_ATTR::FREE_BLOCK;
		}
	}

	MemoryElement*
	findFreeBlock(const u64 size)
	{
		const u64 needSize = sizeof(MemoryElement) + size;
		MemoryElement* e = getRoot();
		while(e) {
#if DEBUG_MEM_LEAK
			catAssert(e->guard == 0xCCCCCCCC, "壊れてる");
#endif
			if((e->attr == BLOCK_ATTR::FREE_BLOCK)
				&& (needSize <= (e->size - sizeof(MemoryElement)))) {
				break;
			}
			e = e->next;
		}
		return e;
	}

	MemoryElement*
	findMemBlock(void* ptr, MemoryElement*& prevBlock)
	{
		prevBlock = nullptr;
		MemoryElement* e = getRoot();
		while(e) {
#if DEBUG_MEM_LEAK
			catAssert(e->guard == 0xCCCCCCCC, "壊れてる");
#endif // DEBUG_MEM_LEAK
			if(ptr == (void*)((u8*)e + sizeof(MemoryElement))) {
				break;
			}
			prevBlock = e;
			e = e->next;
		}
		return e;
	}

public:
	void init() noexcept
	{
		MemoryElement* root = getRoot();
#if DEBUG_MEM_LEAK
		root->guard = 0xCCCCCCCC;
#endif // DEBUG_MEM_LEAK
		root->next = nullptr;
		root->size = BlockSize;
		root->attr = BLOCK_ATTR::FREE_BLOCK;
	}

	/**
	 * @brief メモリを確保する
	 * 
	 * @param[in]	size	確保するメモリサイズ（バイト単位）
	 * @return	確保したメモリ
	 */
	void*
	alloc(const u64 size) noexcept
	{
		// 空きブロックを探す
		MemoryElement* e = findFreeBlock(size);
		if(!e) [[unlikely]] { return nullptr; }
#if DEBUG_MEM_LEAK
		catAssert(e->guard == 0xCCCCCCCC, "壊れてる");
#endif // DEBUG_MEM_LEAK
		// 空きブロックを分割する
		MemoryElement* u = splitBlock(e, size);
		if(!u) [[unlikely]] { return nullptr; }
#if DEBUG_MEM_LEAK
		catAssert(u->guard == 0xCCCCCCCC, "壊れてる");
		u->signiture = alloc_counter;
#endif // DEBUG_MEM_LEAK
		// ヘッダを飛ばしてデータ部分を返す
		return (void*)((u8*)u + sizeof(MemoryElement));
	}

	void
	free(void* ptr) noexcept
	{
		// ブロックを探す
		MemoryElement* prevBlock;
		MemoryElement* e = findMemBlock(ptr, prevBlock);
#if defined(catAssert)
		catAssert(e, "確保していないメモリを解放しようとしました。");
#endif // defined(catAssert)
		if(!e) [[unlikely]] { return; }

		for(u64 i = sizeof(MemoryElement); i < e->size; i++) {
			((u8*)e)[i] = 0xFF;
		}
		// ブロックをくっつける
		fusionBlock(e, prevBlock);
	}

	u64 dump() noexcept {
		u64 counter = 0;
		MemoryElement* e = getRoot();
		while(e) {
			if(e->attr == BLOCK_ATTR::USED_BLOCK) {
				++counter;
			}
			e = e->next;
		}
		return counter;
	}

#if DEBUG_MEM_LEAK
	void check()
	{
		MemoryElement* e = getRoot();
		while(e) {
			catAssert(e->guard == 0xCCCCCCCC, "壊れてる");
			e = e->next;
		}
	}
#endif // DEBUG_MEM_LEAK
};

struct HeapManager {
	/**
	 * @brief ヒープの開始アドレス
	 */
	void* heapBase;

	/**
	 * @brief ヒープサイズ
	 */
	u64	heapSize;

	/**
	 * @brief 固定長のメモリブロック
	 */
	SmallMemoryBlock1 smallBlockMemory1a;
	SmallMemoryBlock1 smallBlockMemory1b;
	SmallMemoryBlock1 smallBlockMemory1c;
	SmallMemoryBlock1 smallBlockMemory1d;
	SmallMemoryBlock2 smallBlockMemory2;
	SmallMemoryBlock3 smallBlockMemory3;
	SmallMemoryBlock4 smallBlockMemory4;
	SmallMemoryBlock5 smallBlockMemory5;

	/**
	 * @brief 可変長のメモリブロック
	 */
	//catLowAllocator<128*1024*1024> blockMemory;
	catLowAllocator<4*1024*1024> blockMemory;
};

#if DEBUG_MEM_LEAK
static_assert(sizeof(HeapManager) <= 512*1024*1024);
#else
//static_assert(sizeof(HeapManager) <= 256*1024*1024);
static_assert(sizeof(HeapManager) <= 16*1024*1024);
#endif

#if !BUILD_WASM
void* getHeapBase() {
	static u8 heapBase[512*1024*1024] = {0};
	static bool first = false;
	if(!first) {
		first = true;
		((HeapManager*)heapBase)->blockMemory.init();
	}
	return heapBase;
}
#else
extern "C" void* __heap_base;
void* getHeapBase() { return __heap_base; }

WASM_EXPORT
void
setupHeap(void* heapBase, size_t heapSize)
{
	__heap_base = heapBase;
	HeapManager* heapMan = (HeapManager*)heapBase;
	heapMan->heapSize = heapSize;
	heapMan->blockMemory.init();
}
#endif

void*
catLowMemAlloc(size_t size)
{
	if(size == 0) { size = 16; }
	size = (size + 15) & ~15;

#if DEBUG_MEM_LEAK
	alloc_counter++;
#endif // DEBUG_MEM_LEAK

	HeapManager* heapMan = (HeapManager*)getHeapBase();
	// 小さいサイズのメモリを扱うもの
	if(size <= heapMan->smallBlockMemory1a.getBlockSize()) {
		if(void* ptr = heapMan->smallBlockMemory1a.alloc(); ptr) { return ptr; }
		if(void* ptr = heapMan->smallBlockMemory1b.alloc(); ptr) { return ptr; }
		if(void* ptr = heapMan->smallBlockMemory1c.alloc(); ptr) { return ptr; }
		if(void* ptr = heapMan->smallBlockMemory1d.alloc(); ptr) { return ptr; }
	}
	if(size <= heapMan->smallBlockMemory2.getBlockSize()) {
		if(void* ptr = heapMan->smallBlockMemory2.alloc(); ptr) { return ptr; }
	}
	if(size <= heapMan->smallBlockMemory3.getBlockSize()) {
		if(void* ptr = heapMan->smallBlockMemory3.alloc(); ptr) { return ptr; }
	}
	if(size <= heapMan->smallBlockMemory4.getBlockSize()) {
		if(void* ptr = heapMan->smallBlockMemory4.alloc(); ptr) { return ptr; }
	}
	if(size <= heapMan->smallBlockMemory5.getBlockSize()) {
		if(void* ptr = heapMan->smallBlockMemory5.alloc(); ptr) { return ptr; }
	}

	// 大きなものは、可変長のブロックで扱う
	return heapMan->blockMemory.alloc(size);
}

void
catLowMemFree(void* ptr)
{
	if(!ptr) [[unlikely]] { return; }
	HeapManager* heapMan = (HeapManager*)getHeapBase();
	// 小さいサイズのメモリを扱うもの
	if(heapMan->smallBlockMemory1a.isMine(ptr)) { return heapMan->smallBlockMemory1a.free(ptr); }
	if(heapMan->smallBlockMemory1b.isMine(ptr)) { return heapMan->smallBlockMemory1b.free(ptr); }
	if(heapMan->smallBlockMemory1c.isMine(ptr)) { return heapMan->smallBlockMemory1c.free(ptr); }
	if(heapMan->smallBlockMemory1d.isMine(ptr)) { return heapMan->smallBlockMemory1d.free(ptr); }
	if(heapMan->smallBlockMemory2.isMine(ptr)) { return heapMan->smallBlockMemory2.free(ptr); }
	if(heapMan->smallBlockMemory3.isMine(ptr)) { return heapMan->smallBlockMemory3.free(ptr); }
	if(heapMan->smallBlockMemory4.isMine(ptr)) { return heapMan->smallBlockMemory4.free(ptr); }
	if(heapMan->smallBlockMemory5.isMine(ptr)) { return heapMan->smallBlockMemory5.free(ptr); }

	// 大きなものは、可変長のブロックで扱う
	heapMan->blockMemory.free(ptr);
}

u64
leakCheck()
{
	u64 leakCounter = 0;
#if DEBUG_MEM_LEAK
	HeapManager* heapMan = (HeapManager*)getHeapBase();

	//heapMan->blockMemory.check();

	leakCounter += heapMan->smallBlockMemory1a.dump();
	leakCounter += heapMan->smallBlockMemory1b.dump();
	leakCounter += heapMan->smallBlockMemory1c.dump();
	leakCounter += heapMan->smallBlockMemory1d.dump();
	leakCounter += heapMan->smallBlockMemory2.dump();
	leakCounter += heapMan->smallBlockMemory3.dump();
	leakCounter += heapMan->smallBlockMemory4.dump();
	leakCounter += heapMan->smallBlockMemory5.dump();
	leakCounter += heapMan->blockMemory.dump();
#endif
	return leakCounter;
}


void* operator new(size_t size) { return catLowMemAlloc(size); }
void operator delete(void* ptr) noexcept { return catLowMemFree(ptr); }
void* operator new[](size_t size) { return catLowMemAlloc(size); }
void operator delete[](void*ptr) noexcept { return catLowMemFree(ptr); }

[[nodiscard]] void* operator new(size_t size, void* ptr) noexcept { return ptr; }
