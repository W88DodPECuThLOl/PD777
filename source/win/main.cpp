#if defined(_WIN32)
#include "win/WinPD777.h"
#include <Windows.h>
#include "win/cat/win/catWinWindowClassEx.h"
#include "win/cat/win/catWinWindow.h"
#include "win/cat/win/catCom.h"
#include <thread>
#include <memory>
#else
static_assert(false, "unsupported target");
#endif // defined(_WIN32)

namespace {

/**
 * @brief コマンドラインのオプション
 */
struct Option {
    std::wstring codeFilename;
    std::wstring patternFilename;
    std::wstring listFilename;
    bool disassemble = false;

    Option()
        : Option(::GetCommandLineW())
    {
    }

    Option(LPWSTR commandLine)
    {
        // 引数リストの取得
        int nArgs = 0;
        auto szArglist = ::CommandLineToArgvW(commandLine, &nArgs);
        if(!szArglist) [[unlikely]] { return; }
        for(int i = 1; i < nArgs; i++) {
            const auto arg = szArglist[i];
            if(arg[0] == L'-') {
                std::wstring opt(arg + 1);
                if((opt == L"code") && ((i + 1) < nArgs)) {
                    codeFilename = szArglist[i + 1];
                    i++;
                } else if((opt == L"ptn") && ((i + 1) < nArgs)) {
                    patternFilename = szArglist[i + 1];
                    i++;
                } else if((opt == L"lst") && ((i + 1) < nArgs)) {
                    listFilename = szArglist[i + 1];
                    i++;
                } else if(opt == L"disassemble") {
                    disassemble = true;
                }
            }
        }
        ::LocalFree(szArglist);
    }
};

/**
 * @brief ファイルを読み込む
 * @param[in]   ファイル名
 * @return 読み込んだファイルのデータ
 */
std::optional<std::vector<u8>>
loadBinaryFile(const std::wstring& filename)
{
    if(filename.empty()) {
        return std::nullopt;
    }
    if(std::unique_ptr<FILE, decltype(&fclose)> fp(_wfopen(filename.c_str(), L"rb"), &fclose); fp) [[likely]] {
        if(fseek(fp.get(), 0, SEEK_END) == 0) [[likely]] {
            if(auto fileSize = ftell(fp.get()); fileSize > 0) [[likely]] {
                if(fseek(fp.get(), 0, SEEK_SET) == 0) [[likely]] {
                    std::vector<u8> data(fileSize);
                    if(fread(data.data(), 1, data.size(), fp.get()) == data.size()) [[likely]] {
                        return std::move(data);
                    }
                }
            }
        }
    }
    return std::nullopt;
}

LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            if(WinPD777* cpu = reinterpret_cast<WinPD777*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA)); cpu) [[likely]] {
                cpu->onPaint(hwnd);
            }
            return 0;
    }
    return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

void
gameThreadEntry(std::uintptr_t param1)
{
    ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    WinPD777* cpu = reinterpret_cast<WinPD777*>(param1);
    while(!cpu->isFinish()) {
        cpu->execute();
    }
}

} // namespace

int
main()
{
    // コマンドラインオプション
    Option option;

    // 必要ならファイルを読み込む
    std::optional<std::vector<u8>> codeData = loadBinaryFile(option.codeFilename);
    if(!option.codeFilename.empty() && !codeData) [[unlikely]] {
        printf("file load error. %ls\n", option.codeFilename.c_str());
        return -1; // 読み込みに失敗した
    }
    std::optional<std::vector<u8>> cgData = loadBinaryFile(option.patternFilename);
    if(!option.patternFilename.empty() && !cgData) [[unlikely]] {
        printf("file load error. %ls\n", option.patternFilename.c_str());
        return -1; // 読み込みに失敗した
    }

    // CPU作成とセットアップ
    std::unique_ptr<WinPD777> cpu = std::make_unique<WinPD777>();
    if(!cpu->setup(codeData, cgData)) [[unlikely]] {
        printf("cpu setup error.\n");
        return -1; // セットアップに失敗した
    }

    if(option.disassemble) [[unlikely]] {
        // 逆アセンブルして終了
        cpu->disassembleAll();
        return 0;
    }

    // COMライブラリの初期化
    cat::win::Com com;
    if (!com) [[unlikely]] {
        printf("CoInitializeEx() failed. Result:0x%08X\n", com.getResult());
        return -1;
    }

    // Windowの作成
    auto hInstance = ::GetModuleHandle(0);
    const std::string className = "CatWindowClassEx";
    auto windowClassEx = std::make_unique<cat::win::WindowClassEx>(hInstance, className, WindowProc);
    const std::string windowName = "μPD777";
    auto window = std::make_unique<cat::win::Window>(hInstance, className, windowName, 375*2, 240*2);

    // ターゲット依存部分のセットアップ
    if(!cpu->targetDependentSetup(window->getWindowHandle())) [[unlikely]] {
        printf("target setup error.\n");
        return -1; // セットアップに失敗した
    }
    window->setWindowUserData(reinterpret_cast<std::uintptr_t>(cpu.get()));

    // ゲームスレッドを立ち上げ
    std::thread gameThread(gameThreadEntry, reinterpret_cast<std::uintptr_t>(cpu.get()));
    // ウィンドウのメッセージループ
    int rc = window->messageLoop();

    // 終了処理
    cpu->setFinish();   // ゲームスレッド側に終了要求
    gameThread.join();  // ゲームスレッドが完了するまで待つ
    return rc;
}
