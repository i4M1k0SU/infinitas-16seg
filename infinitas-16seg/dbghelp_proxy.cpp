#include "pch.h"
#include "dbghelp_proxy.h"

static HINSTANCE hDbghelp;
static BOOL(WINAPI* MakeSureDirectoryPathExists_orig)(PCSTR DirPath) = NULL;

extern "C" _declspec (dllexport) BOOL WINAPI MakeSureDirectoryPathExists(PCSTR DirPath)
{
    return MakeSureDirectoryPathExists_orig(DirPath);
}

bool proxy::dbghelp::load()
{
    TCHAR path[_MAX_PATH];
    if (GetSystemDirectory(path, _MAX_PATH) == 0)
    {
        return false;
    }
    
    _tcscat_s(path, L"\\dbghelp.dll");
    hDbghelp = LoadLibrary(path);
    if (hDbghelp == NULL)
    {
        std::cerr << "Failed: LoadLibrary(dbghelp.dll)" << std::endl;

        return false;
    }

    MakeSureDirectoryPathExists_orig = reinterpret_cast<decltype(MakeSureDirectoryPathExists_orig)>(GetProcAddress(hDbghelp, "MakeSureDirectoryPathExists"));
    if (MakeSureDirectoryPathExists_orig == NULL)
    {
        std::cerr << "Failed: GetProcAddress(dbghelp.dll, MakeSureDirectoryPathExists)" << std::endl;

        return false;
    }

    return true;
}

bool proxy::dbghelp::unload()
{
    if (hDbghelp != NULL && FreeLibrary(hDbghelp) == FALSE)
    {
        return false;
    }

    return true;
}
