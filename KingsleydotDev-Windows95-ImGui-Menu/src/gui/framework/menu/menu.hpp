#pragma once

namespace ksd
{
    bool BeginMenuBar();
    void EndMenuBar();

    bool BeginMenu(const char* label);
    void EndMenu();

    bool MenuItem(const char* label, const char* shortcut = nullptr, bool selected = false, bool enabled = true);
    void MenuSeparator();

    void OpenPopupMenu(const char* id, float x, float y);
    bool BeginPopupMenu(const char* id);
    void EndPopupMenu();

    bool MenuIsOpen();
}
