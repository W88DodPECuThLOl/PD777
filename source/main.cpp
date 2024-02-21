#if defined(_WIN32)
#include "win/WinPD777.h"
#include <Windows.h>
#include "win/cat/win/catWinWindowClassEx.h"
#include "win/cat/win/catWinWindow.h"
#include <thread>
#include <memory>
#else
static_assert(false, "unsupported target");
#endif // defined(_WIN32)

LRESULT CALLBACK
WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    WinPD777* cpu = reinterpret_cast<WinPD777*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            if(cpu) { cpu->onPaint(hwnd); }
            return 0;
    }
    return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

void gameThreadEntry(uintptr_t param1, uintptr_t param2)
{
    SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );

    WinPD777* cpu = (WinPD777*)param1;
    while(!cpu->isFinish()) {
        cpu->execute();
    }
}

int main()
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    auto hInstance = GetModuleHandle(0);

    const std::string className = "CatWindowClassEx";
    std::unique_ptr<cat::win::WindowClassEx> windowClassEx = std::make_unique<cat::win::WindowClassEx>(hInstance, className, WindowProc);
    const std::string windowName = "μPD777";
    std::unique_ptr<cat::win::Window> window = std::make_unique<cat::win::Window>(hInstance, className, windowName, 375*2, 240*2);
    std::unique_ptr<WinPD777> cpu = std::make_unique<WinPD777>(window->getWindowHandle());
    cpu->init();
    SetWindowLongPtr(window->getWindowHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cpu.get()));
    std::thread gameThread(gameThreadEntry, reinterpret_cast<uintptr_t>(cpu.get()), reinterpret_cast<uintptr_t>(window->getWindowHandle()));

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    cpu->setFinish();
    gameThread.join();
    return (int) msg.wParam;
}
