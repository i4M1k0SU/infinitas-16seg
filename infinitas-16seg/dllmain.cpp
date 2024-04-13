#include "pch.h"
#include "dbghelp_proxy.h"
#include "ticker.h"
#include "file_hook.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "setupapi.lib")

static TCHAR szExeFileName[_MAX_FNAME + _MAX_EXT];
static TCHAR szModuleFileName[_MAX_FNAME + _MAX_EXT];

static bool isBm2dx() noexcept
{
#pragma warning(push)
#pragma warning(disable: 26485)
    return _tcscmp(szExeFileName, _T("bm2dx.exe")) == 0;
#pragma warning(pop)
}

static bool isDbghelp() noexcept
{
#pragma warning(push)
#pragma warning(disable: 26485)
    return _tcscmp(szModuleFileName, _T("dbghelp.dll")) == 0;
#pragma warning(pop)
}

#ifdef _DEBUG
static void createConsole() noexcept
{
    FILE* fp;
    ::AllocConsole();
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
}

static void destroyConsole() noexcept
{
    ::FreeConsole();
}
#endif // _DEBUG

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
#ifdef _DEBUG
        createConsole();
#endif // _DEBUG

#pragma warning(push)
#pragma warning(disable: 26485)
        // 呼び出し元のexe名を取得する
        if (::_tcslen(szExeFileName) == 0)
        {
            TCHAR szFilePath[_MAX_PATH];
            ::lstrcpy(
                szExeFileName,
                ::GetModuleFileName(NULL, szFilePath, _MAX_PATH) > 0 ?
                ::PathFindFileName(szFilePath) : _T("error")
            );
        }

        // 自分自身のdll名を取得する
        if (::_tcslen(szModuleFileName) == 0)
        {
            TCHAR szFilePath[_MAX_PATH];
            ::lstrcpy(
                szModuleFileName,
                ::GetModuleFileName(hModule, szFilePath, _MAX_PATH) > 0 ?
                ::PathFindFileName(szFilePath) : _T("error")
            );
        }
#pragma warning(pop)

        if (isDbghelp())
        {
            proxy::dbghelp::load();
        }
        if (!isBm2dx())
        {
            return TRUE;
        }
        std::cout << "Infinitas 16Seg DLL" << std::endl;
        std::cout << "Attach bm2dx.exe" << std::endl;
        ticker::init();
        hook::file::attach();

        return TRUE;
    }

    if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        if (isDbghelp())
        {
            proxy::dbghelp::unload();
        }
        if (!isBm2dx())
        {
            return TRUE;
        }
        std::cout << "Detach bm2dx.exe" << std::endl;
        ticker::close();
        hook::file::detach();

#ifdef _DEBUG
        destroyConsole();
#endif // _DEBUG

        return TRUE;
    }

    return TRUE;
}
