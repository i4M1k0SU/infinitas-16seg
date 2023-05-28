#include "pch.h"
#include "dbghelp_proxy.h"
#include "ticker.h"
#include "file_hook.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "setupapi.lib")

#define BM2DX_EXE _T("bm2dx.exe")
#define DBGHELP_DLL _T("dbghelp.dll")

static TCHAR szExeFileName[_MAX_FNAME + _MAX_EXT];
static TCHAR szModuleFileName[_MAX_FNAME + _MAX_EXT];

static bool isBm2dx()
{
    return _tcscmp(szExeFileName, BM2DX_EXE) == 0;
}

static bool isDbghelp()
{
    return _tcscmp(szModuleFileName, DBGHELP_DLL) == 0;
}

#ifdef DEBUG
static void createConsole()
{
    FILE* fp;
    AllocConsole();
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
}

static void destroyConsole()
{
    FreeConsole();
}
#endif // DEBUG

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    // 呼び出し元のexe名を取得する
    if (_tcslen(szExeFileName) == 0)
    {
        TCHAR szFilePath[_MAX_PATH];
        lstrcpy(
            szExeFileName,
            GetModuleFileName(NULL, szFilePath, _MAX_PATH) > 0 ?
                PathFindFileName(szFilePath) : _T("error")
        );
    }

    // 自分自身のdll名を取得する
    if (_tcslen(szModuleFileName) == 0)
    {
        TCHAR szFilePath[_MAX_PATH];
        lstrcpy(
            szModuleFileName,
            GetModuleFileName(hModule, szFilePath, _MAX_PATH) > 0 ?
                PathFindFileName(szFilePath) : _T("error")
        );
    }

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
#ifdef DEBUG
        createConsole();
#endif // DEBUG
        if (isDbghelp()) proxy::dbghelp::load();
        if (!isBm2dx()) break;
        std::cout << "Infinitas 16Seg DLL" << std::endl;
        std::cout << "Attach bm2dx.exe" << std::endl;
        ticker::init();
        hook::file::attach();
        break;
    case DLL_PROCESS_DETACH:
        if (isDbghelp()) proxy::dbghelp::unload();
        if (!isBm2dx()) break;
        std::cout << "Detach bm2dx.exe" << std::endl;
        hook::file::detach();
        ticker::close();
#ifdef DEBUG
        destroyConsole();
#endif // DEBUG
        break;
    }
    return TRUE;
}
