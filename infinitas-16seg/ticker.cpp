#include "pch.h"
#include "ticker.h"

extern "C" {
#include "16seg_htlab.h"
}

static SegLed segLed;
static std::string displayText = "";
static bool scroll = false;
static bool loop = false;
static size_t prePaddingCnt = 2;

// @see https://github.com/htlabnet/16_Segment_9_Digit_Display_Controller_For_v2/blob/master/Firmware/16_Segment_9_Digit_Display_Controller_For_v2.X/segFonts.h#L23
static char replace16SegChar(char input)
{
    if (input >= 0x00 && input <= 0x20)
    {
        return ' ';
    }

    switch (input)
    {
    // 特殊文字
    case '\'':
        return 'q';
    case ',':
        return 'u';
    case '.':
        return 'm';
    case '`':
        return 'w';
    case '{':
        return '[';
    //case '|':
    //    return '1';
    case '}':
        return ']';

    // 空白が定義されている
    case '#':
    case ':':
    case ';':
    case '@':
    //case 'd':
    //case 'e':
    //case 'f':
    //case '`': // 特殊文字
    //case '{': // 特殊文字
    case '|': // 特殊文字
    //case '}': // 特殊文字
    case '~': // 特殊文字
    case 127: // 特殊文字
    case 128:
    case 129:
        return ' ';
    }

    // 特殊文字
    // `: *の5個版(☆?)
    // a: 〆
    // b: 古
    // c: 平
    // g: 全点灯
    // h-w(x): 1ヶ所点灯
    // x(pと同じ)yz{|}~0x7f(gと同じ): xから0x7fまでで1ヶ所づつ点灯が増えていく

    return toupper(input);
}

static void _loop()
{
    bool firstScroll = true;

    while (loop)
    {
        Sleep(100);
        if (scroll)
        {
            std::string currentText = displayText;
            // 実機の挙動は2文字空けて流れ始める
            // 流れ切った後、次が流れるまでに2文字分のラグがある
            std::string tmp = (firstScroll ? std::string(prePaddingCnt, ' ') : "         ") + displayText + "           ";
            if (firstScroll)
            {
                firstScroll = false;
            }

            for (size_t i = 0; i < tmp.size() - 9; i++)
            {
                if (currentText != displayText)
                {
                    firstScroll = true;
                    break;
                }
                writeSegLed(&segLed, tmp.substr(i, 9).c_str());
                Sleep(320);
            }

            continue;
        }
        writeSegLed(&segLed, displayText.substr(0, 9).c_str());
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
    displayScroll("NOW LOADING...");

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

void ticker::display(std::string text, bool isAppend)
{
    transform(text.begin(), text.end(), text.begin(), ::replace16SegChar);
    scroll = false;
    if (isAppend)
    {
        displayText += text;
    }
    else
    {
        displayText = text;
    }
}

void ticker::displayScroll(std::string text, bool isAppend, size_t prePadding)
{
    transform(text.begin(), text.end(), text.begin(), ::replace16SegChar);
    scroll = true;
    prePaddingCnt = prePadding;
    if (isAppend)
    {
        displayText += text;
    }
    else
    {
        displayText = text;
    }
}
