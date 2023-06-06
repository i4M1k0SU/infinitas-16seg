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
static void replace16SegStr(std::string& text)
{
    std::string asterisk = "*";
    std::string star = "\x81\x99"; // ☆ in Shift_JIS
    size_t starSize = star.size();

    for (size_t i = 0; i < text.size(); i++)
    {
        // ☆ -> * の置換 (英語表記に ☆ が含まれている曲がある)
        // 実機での☆の扱いは、 ` (*の5個版) ではなく、 * が使われている
        if (text.substr(i, starSize) == star)
        {
            text.replace(i, starSize, asterisk);
            i += asterisk.size() - 1;
        }

        // 特殊制御文字・大文字置換
        char& c = text[i];
        if (c >= 0x00 && c <= 0x20)
        {
            c = ' ';
            continue;
        }

        switch (c)
        {
        // 特殊文字
        case '\'': c = 'q'; continue;
        case ',': c = 'u'; continue;
        case '.': c = 'm'; continue;
        case '`': c = 'w'; continue;
        case '{': c = '['; continue;
        //case '|': c = '1'; continue;
        case '}': c = ']'; continue;
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
            c = ' ';
            continue;
        }

        // 特殊文字
        // `: *の5個版(☆?)
        // a: 〆
        // b: 古
        // c: 平
        // g: 全点灯
        // h-w(x): 1ヶ所点灯
        // x(pと同じ)yz{|}~0x7f(gと同じ): xから0x7fまでで1ヶ所づつ点灯が増えていく

        c = toupper(c);
    }
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
    replace16SegStr(text);
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
    replace16SegStr(text);
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
