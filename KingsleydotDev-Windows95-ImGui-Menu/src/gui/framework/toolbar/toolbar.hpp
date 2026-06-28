#pragma once

namespace ksd
{
    void BeginToolbar();
    void EndToolbar();

    bool ToolButton(const char* id, int glyph, const char* tooltip = nullptr, bool enabled = true);
    void ToolSeparator();
}
