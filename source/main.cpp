#if defined(_WIN32)
#include "win/WinPD777.h"
#include <Windows.h>
#else
static_assert(false, "unsupported target");
#endif // defined(_WIN32)

namespace {

void
setConsoleCursorVisible(const bool visible)
{
#if defined(_WIN32)
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hOut, &cci);
	cci.bVisible = visible ? TRUE : FALSE;
	SetConsoleCursorInfo(hOut, &cci);
#endif // defined(_WIN32)
}

void
setConsoleTerminalMode()
{
#if defined(_WIN32)
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif // defined(_WIN32)
}

void
setupConsole()
{
    setConsoleTerminalMode();
    setConsoleCursorVisible(false);
#if defined(_WIN32)
	HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
#endif
}

} // namespace

[[noreturn]]
int main()
{
    setupConsole();

    WinPD777 cpu;
    cpu.init();
    //cpu.disassembleAll();
    for(;;) {
        cpu.execute();
    }
}
