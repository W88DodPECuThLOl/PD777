rem WebAssembly用にオブジェクトを生成
if not exist build mkdir build
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../source/core/catLowMemory.cpp -o ./build/catLowMemory.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../source/core/Decoder.cpp      -o ./build/Decoder.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../source/core/Disassembler.cpp -o ./build/Disassembler.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../source/core/PD777.cpp        -o ./build/PD777.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../source/core/rom.cpp          -o ./build/rom.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../source/core/Stack.cpp        -o ./build/Stack.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../source/wasm/WasmPD777.cpp    -o ./build/WasmPD777.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../source/wasm/wasmSupport.cpp  -o ./build/wasmSupport.o
rem リンクしてupd777.wasmを作成
clang "-Wl,--no-entry" "-Wl,--export-all" "-Wl,--import-memory" -fno-builtin -nostdlib --target=wasm32 -o upd777.wasm ./build/*.o
