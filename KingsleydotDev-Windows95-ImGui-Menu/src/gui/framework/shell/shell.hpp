#pragma once

#include "imgui.h"

namespace ksd
{
    enum class HostMode
    {
        Standalone,
        Embedded
    };

    void SetWindowHandle(void* hwnd);
    void SetHostMode(HostMode mode);
    HostMode GetHostMode();
    bool CloseRequested();

    void BeginMain(const char* title);
    void EndMain();

    void SetColumns(int count);
    bool TabButton(const char* label, int index);

    bool BeginGroup(const char* label, int column);
    void EndGroup();

    ImVec2 CursorPos();
    void   AddItem(const ImVec2& size);
    bool   IsLastItemHovered();
    void   Indent(float w = 14.f);
    void   Unindent(float w = 14.f);
    float  ContentWidth();
    float  PanelLeftX();
    float  PanelRightX();

    float  BodyTop();
    float  BodyBottom();
    float  BodyLeft();
    float  BodyRight();

    float  MenuBarTop();
    float  ToolbarTop();
    void   SetToolbarVisible(bool visible);

    void   SetAccent(const float col[4], bool enabled);
    ImU32  AccentColor();

    bool   PopUpsAreClosed();
    const char* SelectedItem();
    void   SetSelectedItem(const char* id);

    void   OpenUrl(const char* url);

    int& CurrentTab();
}
