#pragma once

#if defined(_WIN32)
#include <Windows.h>
#include <string>

namespace cat::win {

class Window {
	HWND hwnd;
public:
	Window(HINSTANCE instance, const std::string& className, const std::string& windowName, const int windowWidth, const int windowHeight)
		: hwnd(NULL)
	{
		constexpr DWORD style = WS_OVERLAPPEDWINDOW;
		RECT windowRect = {0, 0, windowWidth, windowHeight};
		AdjustWindowRect( &windowRect, style, FALSE );
		hwnd = CreateWindowEx(
			0,                              // Optional window styles.
			className.c_str(),              // Window class
			windowName.c_str(),    // Window text
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,            // Window style
			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,

			NULL,       // Parent window    
			NULL,       // Menu
			instance,  // Instance handle
			NULL        // Additional application data
		);
		if(hwnd) {
//			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
			ShowWindow(hwnd, SW_SHOWDEFAULT);
			UpdateWindow(hwnd);
		}
	}
	~Window()
	{
		if(hwnd) {
			DestroyWindow(hwnd);
		}
	}

	HWND getWindowHandle() const noexcept { return hwnd; }
};

} // cat::win

#endif // defined(_WIN32)
