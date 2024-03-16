#pragma once

#if defined(_WIN32)

#include <Windows.h>

namespace cat::win {

struct Com {
    HRESULT hr;
    Com(DWORD dwCoInit = COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE) : hr(::CoInitializeEx(NULL, dwCoInit)) {}
    Com(const Com&) = delete;
    Com(const Com&&) = delete;
    ~Com() { if(SUCCEEDED(hr)) { ::CoUninitialize(); } }
    Com& operator=(const Com&) = delete;
    explicit operator bool() const { return SUCCEEDED(hr); }
    HRESULT getResult() const { return hr; }
};

} // namespace cat::win

#endif // defined(_WIN32)
