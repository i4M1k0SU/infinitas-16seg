#include "pch.h"
#include "file_hook.h"
#include "ticker_handler.h"

static uint32_t(*k_fopen)(const char* filename, uint32_t flag, uint32_t mode);
static size_t(*k_fread)(uint32_t fp, uint8_t* buf, size_t size);

static bool isHookEnabled = false;
static HINSTANCE hAvs2Core;
static uint32_t musicDataFp = 0;

static uint32_t k_fopen_hook(const char* filename, uint32_t flag, uint32_t mode)
{
    // 無駄な処理を省くために /data で始まるファイルだけを絞る
    if (filename[0] == '/' && filename[1] == 'd' && filename[2] == 'a' && filename[3] == 't' && filename[4] == 'a' && filename[5] == '/')
    {
        std::string filename_str = std::string(filename);

        if (filename_str == "/data/info/music_data.bin" && ticker::handler::isEmptyMusicTitleMap())
        {
            musicDataFp = k_fopen(filename, flag, mode);
            return musicDataFp;
        }

        ticker::handler::fopenHandler(filename_str);
    }

    return k_fopen(filename, flag, mode);
}

static size_t k_fread_hook(uint32_t fp, uint8_t* buf, size_t size)
{
    size_t readSize = k_fread(fp, buf, size);

    // サイズは適当, magic number判定
    if (fp == musicDataFp && readSize > 1000000 && buf[0] == 0x49 && buf[1] == 0x49 && buf[2] == 0x44 && buf[3] == 0x58)
    {
        // 0x278e0 bytes から楽曲情報が始まり、 0x3f0 bytes ごとに繰り返される
        for (size_t i = 0x278e0; i < readSize; i += 0x3f0)
        {
            uint8_t* buffer = buf + i;
            // 0x270 からリトルエンディアンで 4 bytes が楽曲IDっぽい
            uint32_t id = buffer[0x270] + (buffer[0x271] << 8) + (buffer[0x272] << 16) + (buffer[0x273] << 24);
            // readSizeでピッタリ終わるようではないので、idが0になったら読み込み終わりとみなす
            if (id == 0)
            {
                break;
            }
            // 0x40 から 64 bytes が英語表記の楽曲名っぽい
            auto title_char = reinterpret_cast<const char*>(buffer + 0x40);
            size_t len = strnlen(title_char, 0x40);
            auto title = std::string(title_char, len);
            ticker::handler::setMusicTitle(id, title);
        }
    }

    return readSize;
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
    k_fread = reinterpret_cast<decltype(k_fread)>(GetProcAddress(hAvs2Core, "XCgsqzn0000051"));

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
    DetourAttach(reinterpret_cast<PVOID*>(&k_fread), static_cast<PVOID>(k_fread_hook));
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
    DetourDetach(reinterpret_cast<PVOID*>(&k_fread), static_cast<PVOID>(k_fread_hook));
    DetourTransactionCommit();

    return true;
}
