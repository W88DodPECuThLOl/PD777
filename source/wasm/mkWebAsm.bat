REM build for WebAssembly

if not exist obj mkdir obj

REM clang version 18.1.1
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../core/Decoder.cpp -o ./obj/Decoder.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../core/Disassembler.cpp -o ./obj/Disassembler.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../core/KeyStatus.cpp -o ./obj/KeyStatus.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../core/PD777.cpp -o ./obj/PD777.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../core/PD777Graphics.cpp -o ./obj/PD777Graphics.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../core/rom.cpp -o ./obj/rom.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../core/romSetup.cpp -o ./obj/romSetup.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ../core/Stack.cpp -o ./obj/Stack.o

clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ./WasmPD777.cpp -o ./obj/WasmPD777.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ./clangSupport.cpp -o ./obj/clangSupport.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ./catLowMemory.cpp -o ./obj/catLowMemory.o
clang -DBUILD_WASM=32 -std=c++20 -O3 -fno-builtin --target=wasm32 -c ./main.cpp -o ./obj/main.o
clang -Wl,--no-entry -Wl,--export-dynamic -Wl,--import-memory -fno-builtin -nostdlib --target=wasm32 -o ./html/wasm/uPD777.wasm ./obj/Decoder.o ./obj/Disassembler.o ./obj/KeyStatus.o ./obj/PD777.o ./obj/PD777Graphics.o ./obj/rom.o ./obj/romSetup.o ./obj/Stack.o ./obj/WasmPD777.o ./obj/clangSupport.o ./obj/catLowMemory.o ./obj/main.o

REM wasm2wat 1.0.34
REM wasm2wat uPD777.wasm -o ./uPD777.wat
