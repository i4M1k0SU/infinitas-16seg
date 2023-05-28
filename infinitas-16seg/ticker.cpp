#include "pch.h"
#include "ticker.h"

extern "C" {
#include "16seg_htlab.h"
}

static SegLed segLed;
static std::string displayString = "";
static bool scroll = false;
static bool loop = false;

static void _loop()
{
    while (loop)
    {
        Sleep(100);
        if (scroll)
        {
            std::string current = displayString;
            // 実機の挙動は流れ切った後、次が流れるまでにラグがある
            // その挙動を再現するため末尾の空白を2文字多くする
            std::string tmp = "         " + displayString + "           ";
            for (size_t i = 0; i < tmp.size() - 9; i++)
            {
                if (current != displayString)
                {
                    break;
                }
                writeSegLed(&segLed, tmp.substr(i, 9).c_str());
                Sleep(320);
            }

            continue;
        }
        writeSegLed(&segLed, displayString.substr(0, 9).c_str());
    }
}

static void beginDisplayLoop()
{
    if (segLed.available)
    {
        std::cout << "begin display loop" << std::endl;
        loop = true;
        std::thread loopThread(_loop);
        loopThread.detach();
    }
}

bool ticker::init()
{
    std::cout << "Initializing 16Seg LED..." << std::endl;
    bool available = initSegLed(&segLed);
    std::cout << "available: " << (available ? "YES" : "NO") << std::endl;
    beginDisplayLoop();
    display("NOW LOADINGmmm", true); // NOW LOADING...

    return available;
}

void ticker::close()
{
    if (segLed.available)
    {
        std::cout << "Closing 16Seg LED..." << std::endl;
        loop = false;
        closeSegLed(&segLed);
    }
}

void ticker::display(std::string text, bool isScroll)
{
    std::cout << text << std::endl;
    if (segLed.available)
    {
        scroll = isScroll;
        displayString = text;
    }
}
