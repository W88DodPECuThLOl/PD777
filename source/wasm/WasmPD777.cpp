#include "WasmPD777.h"
#include "../core/catLowWasmExportMacro.h"

WASM_IMPORT("pd777","present")
void wasmPresent(void* frameBuffer);

#if !defined(BUILD_WASM)
void wasmPresent(void* frameBuffer) {}
#endif // !defined(BUILD_WASM)

void
WasmPD777::present()
{
	wasmPresent(frameBuffer);
}
