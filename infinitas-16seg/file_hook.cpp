#include "pch.h"
#include "file_hook.h"
#include "ticker_handler.h"

static constexpr std::array<uint8_t, 4> MUSIC_DATA_HEADER = { 0x49, 0x49, 0x44, 0x58 };

static uint32_t(*k_fopen)(const char* filename, uint32_t flag, uint32_t mode);
static size_t(*k_fread)(uint32_t fp, uint8_t* buf, size_t size);

static bool isHookEnabled = false;
static HINSTANCE hAvs2Core;
static uint32_t musicDataFp = 0;

static uint32_t k_fopen_hook(const char* filename, uint32_t flag, uint32_t mode)
{
    // 無駄な処理を省くために /data で始まるファイルだけを絞る
    if (::strncmp(filename, "/data/", 6) == 0)
    {
        std::string filenameStr = std::string(filename);

        if (filenameStr == "/data/info/music_data.bin" && ticker::handler::isEmptyMusicTitleMap())
        {
            musicDataFp = k_fopen(filename, flag, mode);
            return musicDataFp;
        }

        ticker::handler::fopenHandler(filenameStr);
    }

    return k_fopen(filename, flag, mode);
}

static size_t k_fread_hook(uint32_t fp, uint8_t* buf, size_t size)
{
    const size_t readSize = k_fread(fp, buf, size);

    // サイズは適当, magic number判定
    if (fp != musicDataFp || readSize <= 1000000 || std::memcmp(buf, MUSIC_DATA_HEADER.data(), MUSIC_DATA_HEADER.size()) != 0)
    {
        return readSize;
    }

    const gsl::span<const uint8_t> buffer(buf, readSize);

    // 0x278e0 bytes から楽曲情報が始まり、 0x3f0 bytes ごとに繰り返される
    for (size_t i = 0x278e0; i < buffer.size(); i += 0x3f0)
    {
        const auto musicInfoBuf = buffer.subspan(i);
        // 0x270 からリトルエンディアンで 4 bytes が楽曲IDっぽい
        const uint32_t id = musicInfoBuf[0x270] + (musicInfoBuf[0x271] << 8) + (musicInfoBuf[0x272] << 16) + (musicInfoBuf[0x273] << 24);
        // readSizeでピッタリ終わるようではないので、idが0になったら読み込み終わりとみなす
        if (id == 0)
        {
            break;
        }
#pragma warning(push)
#pragma warning(disable: 26490)
        // 0x40 から 64 bytes が英語表記の楽曲名っぽい
        const auto titleChar = reinterpret_cast<const char*>(&musicInfoBuf[0x40]);
#pragma warning(pop)
        const size_t len = strnlen(titleChar, 0x40);
        const auto title = std::string(titleChar, len);
        ticker::handler::setMusicTitle(id, title);
    }

    return readSize;
}

bool hook::file::attach()
{
    hAvs2Core = ::LoadLibrary(L"avs2-core.dll");
    if (hAvs2Core == NULL)
    {
        std::cerr << "Failed: LoadLibrary(avs2-core.dll)" << std::endl;
        return false;
    }

#pragma warning(push)
#pragma warning(disable: 26490)
    k_fopen = reinterpret_cast<decltype(k_fopen)>(::GetProcAddress(hAvs2Core, "XCgsqzn000004e"));
    k_fread = reinterpret_cast<decltype(k_fread)>(::GetProcAddress(hAvs2Core, "XCgsqzn0000051"));

    if (k_fopen == NULL)
    {
        std::cerr << "Failed: GetProcAddress(avs2-core.dll)" << std::endl;
        return false;
    }

    if (::DetourIsHelperProcess())
    {
        return false;
    }

    ::DetourRestoreAfterWith();
    ::DetourTransactionBegin();
    ::DetourUpdateThread(::GetCurrentThread());
    ::DetourAttach(reinterpret_cast<PVOID*>(&k_fopen), gsl::narrow<PVOID>(k_fopen_hook));
    ::DetourAttach(reinterpret_cast<PVOID*>(&k_fread), gsl::narrow<PVOID>(k_fread_hook));
#pragma warning(pop)
    if (::DetourTransactionCommit() != NO_ERROR)
    {
        return false;
    }

    isHookEnabled = true;

    return true;
}

bool hook::file::detach()
{
    if (!isHookEnabled) return true;

    ::DetourTransactionBegin();
    ::DetourUpdateThread(::GetCurrentThread());
#pragma warning(push)
#pragma warning(disable: 26490)
    ::DetourDetach(reinterpret_cast<PVOID*>(&k_fopen), gsl::narrow<PVOID>(k_fopen_hook));
    ::DetourDetach(reinterpret_cast<PVOID*>(&k_fread), gsl::narrow<PVOID>(k_fread_hook));
#pragma warning(pop)
    ::DetourTransactionCommit();
    ::FreeLibrary(hAvs2Core);

    return true;
}
