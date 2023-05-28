#include "pch.h"
#include "file_hook.h"
#include "ticker.h"

static bool isHookEnabled = false;
static HINSTANCE hAvs2Core;
static uint32_t(*k_fopen)(const char* filename, uint16_t mode, int flags);

static uint32_t k_fopen_hook(const char* filename, uint16_t mode, int flags)
{
    //std::cout << "filename: " << filename << std::endl;

    if (filename[0] == '/' && filename[1] == 'd' && filename[2] == 'a' && filename[3] == 't' && filename[4] == 'a')
    {
        std::string filename_str = std::string(filename);

        if (filename_str == "/data/graphic/logo.ifs")
        {
            ticker::display("WELCOME TO BEATMANIA IIDX INFINITAS", true);

        }
        if (filename_str == "/data/graphic/card.ifs")
        {
            ticker::display(" WELCOME ");
            std::thread t([]() {
                Sleep(1500);
                ticker::display("  ENTRY  ");
            });
            t.detach();
        }
        if (filename_str == "/data/graphic/basic_frame.ifs")
        {
            ticker::display(" DECIDE! ");
        }
        if (filename_str == "/data/graphic/mode.ifs") // /data/graphic/ticket_panel.ifs
        {
            ticker::display("MODE?", true);
        }
        if (filename_str == "/data/graphic/mselect.ifs")
        {
            ticker::display("STAY COOL");
        }
        if (filename_str == "/data/graphic/mlist_window.ifs")
        {
            ticker::display("MUSIC SELECT!!", true);
        }
    }

    return k_fopen(filename, mode, flags);
}

bool hook::file::attach()
{
    hAvs2Core = LoadLibrary(L"avs2-core.dll");
    if (hAvs2Core == NULL)
    {
        std::cerr << "Failed: LoadLibrary(avs2-core.dll)" << std::endl;
        return false;
    }

    k_fopen = reinterpret_cast<decltype(k_fopen)>(GetProcAddress(hAvs2Core, "XCgsqzn000004e"));

    if (k_fopen == NULL)
    {
        std::cerr << "Failed: GetProcAddress(avs2-core.dll)" << std::endl;
        return false;
    }

    if (DetourIsHelperProcess())
    {
        return false;
    }

    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(reinterpret_cast<PVOID*>(&k_fopen), static_cast<PVOID>(k_fopen_hook));
    if (DetourTransactionCommit() != NO_ERROR)
    {
        return false;
    }

    isHookEnabled = true;

    return true;
}

bool hook::file::detach()
{
    if (!isHookEnabled) return true;

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(reinterpret_cast<PVOID*>(&k_fopen), static_cast<PVOID>(k_fopen_hook));
    DetourTransactionCommit();

    return true;
}
