#pragma once

namespace ticker::handler
{
    void fopenHandler(std::string filename);
    void setMusicTitle(uint32_t id, std::string title);
    bool isEmptyMusicTitleMap();
}
