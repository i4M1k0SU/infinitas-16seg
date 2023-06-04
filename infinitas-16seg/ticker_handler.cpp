#include "pch.h"
#include "ticker_handler.h"
#include "ticker.h"

static bool isEntered = false;
static bool isPlayed = false;
static bool isMode = false;
static bool isDecideDisplayed = false;
static std::map<uint32_t, std::string> musicTitles;
static std::regex pathPattern("^/data/sound/(preview/|)([0-9]{5})(_pre\\.2dx|/.+)$");
static std::smatch match;

// ファイル名からの取得は不可能である項目は変更している
// @see https://youtu.be/FS8T7abHsuQ
// @see https://youtu.be/xHBT0c2PS0c
void ticker::handler::fopenHandler(std::string filename)
{
    if (filename == "/data/graphic/logo.ifs")
    {
        // 完全に右から流れる (padding 9)
        ticker::displayScroll("WELCOME TO BEATMANIA IIDX INFINITAS", false, 9);
        return;

    }

    if (filename == "/data/graphic/card.ifs")
    {
        if (!isEntered)
        {
            ticker::display("  ENTRY  ");
            isEntered = true;
            return;
        }
        ticker::displayScroll("THANK YOU FOR PLAYING!!");
        isEntered = false;
        return;
    }

    // 本来はSP/DP切り替え後に出るが、ENTRY後に出るようにしている
    // INFINITASの挙動ではMODE SELECTに戻ってもENTRY画面には二度と戻ることがないので一度だけ出るようにする
    if (filename == "/data/graphic/basic_frame.ifs" && !isDecideDisplayed)
    {
        ticker::display(" DECIDE! ");
        isDecideDisplayed = true;
        return;
    }

    // ticket_panel.ifs の方がタイミングは正しいが、リザルトから選曲画面に戻った時も呼ばれるためisModeを使う
    if (filename == "/data/graphic/mode.ifs")
    {
        isMode = true;
        isEntered = true;
        isPlayed = false;
        return;
    }

    // mode.ifs を読み込んだ時のみ
    if (filename == "/data/graphic/ticket_panel.ifs" && isMode)
    {
        ticker::display("  MODE?  ");
        isMode = false;
        return;
    }

    // 本来はCLASSの時にも出す
    if (filename == "/data/graphic/mselect.ifs" && !isPlayed)
    {
        ticker::display("STAY COOL");
        return;
    }

    if (filename == "/data/graphic/mlist_window.ifs")
    {
        ticker::displayScroll("MUSIC SELECT!!");
        isPlayed = true;
        return;
    }

    // [曲名] CLEAR! or FAILED..
    if (filename == "/data/graphic/result.ifs")
    {
        ticker::displayScroll(" RESULT", true);
        return;
    }

    // 10TH DAN, CHUDEN など
    if (filename == "/data/graphic/dan_select.ifs")
    {
        ticker::displayScroll("CLASS");
        isPlayed = true;
        return;
    }

    // GOOD LUCK

    // SUCCESS or UNSUCCESS
    if (filename == "/data/graphic/d_result.ifs")
    {
        ticker::displayScroll("CLASS RESULT");
        return;
    }

    if (std::regex_match(filename, match, pathPattern) && match.size() > 2)
    {
        uint32_t id = std::stoi(match[2].str(), nullptr, 10);
        if (musicTitles.contains(id))
        {
            ticker::displayScroll(musicTitles[id]);
        }
        return;
    }
}

void ticker::handler::setMusicTitle(uint32_t id, std::string title)
{
    musicTitles[id] = title;
    std::cout << "id: " << id << " title: " << title << std::endl;
}

bool ticker::handler::isEmptyMusicTitleMap()
{
    return musicTitles.empty();
}
