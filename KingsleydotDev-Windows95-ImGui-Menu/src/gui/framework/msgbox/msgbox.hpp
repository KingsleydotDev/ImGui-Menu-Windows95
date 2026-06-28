#pragma once

namespace ksd
{
    struct MessageLink
    {
        const char* label;
        const char* url;
    };

    void OpenMessageBox(const char* id, const char* title, const char* text);
    void OpenMessageBox(const char* id, const char* title, const char* text, const MessageLink* links, int linkCount);

    void DrawMessageBox();

    int MessageBoxResult(const char* id);
}
