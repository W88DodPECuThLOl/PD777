#pragma once

#if defined(_WIN32)
#include <Windows.h>
#include <string>

namespace cat::win {

//! @brief WindowClass
class WindowClassEx {
	HINSTANCE instance;
	std::string className;
public:
	WindowClassEx(HINSTANCE instance_, const std::string& className_, WNDPROC windowProcedure)
		: instance(instance_)
		, className(className_)
	{
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = windowProcedure;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = sizeof(uintptr_t);
		wc.hInstance = instance;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = className.c_str();
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		RegisterClassEx(&wc);
	}
	~WindowClassEx()
	{
		UnregisterClass(className.c_str(), instance);
	}
};

} // cat::win

#endif // defined(_WIN32)
