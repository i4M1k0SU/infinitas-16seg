#pragma once

namespace ticker
{
    bool init();
    void close();
    void display(std::string, bool isAppend = false);
    void displayScroll(std::string text, bool isAppend = false, size_t prePadding = 2);
}
