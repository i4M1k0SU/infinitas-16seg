#pragma once

namespace ticker
{
    bool init();
    void close();
    void display(std::string, bool isScroll = false);
}
