#pragma once

namespace ticker::handler
{
    void fopenHandler(const std::string& filename);
    void setMusicTitle(uint32_t id, const std::string& title);
    bool isEmptyMusicTitleMap() noexcept;
}
