#include "pch.h"
#include "dbghelp_proxy.h"

static HINSTANCE hDbghelp;
static FARPROC p_MakeSureDirectoryPathExists;
static FARPROC p_StackWalk64;
static FARPROC p_SymCleanup;
static FARPROC p_SymGetLineFromAddr64;
static FARPROC p_SymGetSymFromAddr64;
static FARPROC p_SymInitialize;
static FARPROC p_MiniDumpWriteDump;

extern "C" {
    _declspec (dllexport) BOOL WINAPI MakeSureDirectoryPathExists() { return p_MakeSureDirectoryPathExists(); }
    _declspec (dllexport) BOOL WINAPI StackWalk64() { return p_StackWalk64(); }
    _declspec (dllexport) BOOL WINAPI SymCleanup() { return p_SymCleanup(); }
    _declspec (dllexport) BOOL WINAPI SymGetLineFromAddr64() { return p_SymGetLineFromAddr64(); }
    _declspec (dllexport) BOOL WINAPI SymGetSymFromAddr64() { return p_SymGetSymFromAddr64(); }
    _declspec (dllexport) BOOL WINAPI SymInitialize() { return p_SymInitialize(); }
    _declspec (dllexport) BOOL WINAPI MiniDumpWriteDump() { return p_MiniDumpWriteDump(); }
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

    p_MakeSureDirectoryPathExists = GetProcAddress(hDbghelp, "MakeSureDirectoryPathExists");
    p_StackWalk64 = GetProcAddress(hDbghelp, "StackWalk64");
    p_SymCleanup = GetProcAddress(hDbghelp, "SymCleanup");
    p_SymGetLineFromAddr64 = GetProcAddress(hDbghelp, "SymGetLineFromAddr64");
    p_SymGetSymFromAddr64 = GetProcAddress(hDbghelp, "SymGetSymFromAddr64");
    p_SymInitialize = GetProcAddress(hDbghelp, "SymInitialize");
    p_MiniDumpWriteDump = GetProcAddress(hDbghelp, "MiniDumpWriteDump");

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
