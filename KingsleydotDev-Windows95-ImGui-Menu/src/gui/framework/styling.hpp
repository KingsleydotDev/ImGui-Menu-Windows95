#pragma once

#include "imgui.h"

namespace style
{
    constexpr ImU32 Face         = IM_COL32(192, 192, 192, 255);
    constexpr ImU32 Light        = IM_COL32(223, 223, 223, 255);
    constexpr ImU32 Highlight    = IM_COL32(255, 255, 255, 255);
    constexpr ImU32 Shadow       = IM_COL32(128, 128, 128, 255);
    constexpr ImU32 DarkShadow   = IM_COL32(0, 0, 0, 255);

    constexpr ImU32 Field        = IM_COL32(255, 255, 255, 255);
    constexpr ImU32 Text         = IM_COL32(0, 0, 0, 255);
    constexpr ImU32 TextDisabled = IM_COL32(128, 128, 128, 255);

    constexpr ImU32 TitleLeft    = IM_COL32(0, 0, 128, 255);
    constexpr ImU32 TitleRight   = IM_COL32(16, 132, 208, 255);
    constexpr ImU32 TitleText    = IM_COL32(255, 255, 255, 255);

    constexpr ImU32 Selection    = IM_COL32(0, 0, 128, 255);
    constexpr ImU32 SelText      = IM_COL32(255, 255, 255, 255);
    constexpr ImU32 SliderFill   = IM_COL32(0, 0, 160, 255);

    constexpr float FrameBorder  = 3.f;
    constexpr float TitleBarH    = 18.f;
    constexpr float TitleBtnW    = 16.f;
    constexpr float TitleBtnH    = 14.f;
    constexpr float MenuBarH     = 18.f;
    constexpr float ToolbarH     = 26.f;
    constexpr float TabBarH      = 21.f;
    constexpr float TabPadX      = 8.f;
    constexpr float StatusBarH   = 20.f;
    constexpr float GripSize     = 16.f;
    constexpr float ScrollbarW     = 16.f;
    constexpr float ScrollArrowBtn = 16.f;
    constexpr float ScrollMinThumb = 16.f;

    constexpr float ContentPad   = 5.f;
    constexpr float ColumnGutter = 5.f;
    constexpr float GroupGap     = 6.f;
    constexpr float GroupTopPad  = 15.f;
    constexpr float GroupInnerX  = 9.f;
    constexpr float GroupBottomPad = 9.f;

    constexpr float RowSpacing   = 4.f;
    constexpr float CheckboxSize = 13.f;
    constexpr float RowHeight    = 14.f;
    constexpr float SwatchW      = 26.f;
    constexpr float SwatchH      = 14.f;
    constexpr float KeybindW     = 34.f;
    constexpr float KeybindH     = 14.f;
    constexpr float SliderTrackH  = 22.f;
    constexpr float ThumbW        = 11.f;
    constexpr float ThumbBodyH    = 13.f;
    constexpr float ThumbPointH   = 5.f;
    constexpr float TickH         = 3.f;
    constexpr float ComboH        = 20.f;
    constexpr float ButtonH       = 23.f;

    inline void FillFace(ImDrawList* dl, const ImVec2& a, const ImVec2& b)
    {
        dl->AddRectFilled(a, b, Face);
    }

    inline void Edge(ImDrawList* dl, const ImVec2& a, const ImVec2& b, ImU32 topLeft, ImU32 bottomRight)
    {
        dl->AddRectFilled(ImVec2(a.x, a.y), ImVec2(b.x, a.y + 1.f), topLeft);
        dl->AddRectFilled(ImVec2(a.x, a.y), ImVec2(a.x + 1.f, b.y), topLeft);
        dl->AddRectFilled(ImVec2(a.x, b.y - 1.f), ImVec2(b.x, b.y), bottomRight);
        dl->AddRectFilled(ImVec2(b.x - 1.f, a.y), ImVec2(b.x, b.y), bottomRight);
    }

    inline void Raised(ImDrawList* dl, const ImVec2& a, const ImVec2& b)
    {
        Edge(dl, a, b, Highlight, DarkShadow);
        Edge(dl, ImVec2(a.x + 1.f, a.y + 1.f), ImVec2(b.x - 1.f, b.y - 1.f), Light, Shadow);
    }

    inline void Pressed(ImDrawList* dl, const ImVec2& a, const ImVec2& b)
    {
        Edge(dl, a, b, Shadow, Highlight);
        Edge(dl, ImVec2(a.x + 1.f, a.y + 1.f), ImVec2(b.x - 1.f, b.y - 1.f), DarkShadow, Face);
    }

    inline void Sunken(ImDrawList* dl, const ImVec2& a, const ImVec2& b)
    {
        Edge(dl, a, b, Shadow, Highlight);
        Edge(dl, ImVec2(a.x + 1.f, a.y + 1.f), ImVec2(b.x - 1.f, b.y - 1.f), DarkShadow, Light);
    }

    inline void Etched(ImDrawList* dl, const ImVec2& a, const ImVec2& b)
    {
        Edge(dl, a, b, Shadow, Highlight);
        Edge(dl, ImVec2(a.x + 1.f, a.y + 1.f), ImVec2(b.x - 1.f, b.y - 1.f), Highlight, Shadow);
    }

    inline void TitleBar(ImDrawList* dl, const ImVec2& a, const ImVec2& b)
    {
        dl->AddRectFilled(a, b, TitleLeft);
    }

    inline void Stipple(ImDrawList* dl, const ImVec2& a, const ImVec2& b, ImU32 c0, ImU32 c1)
    {
        dl->AddRectFilled(a, b, c0);
        int row = 0;
        for (float y = a.y; y < b.y; y += 1.f, ++row)
        {
            for (float x = a.x + ((row & 1) ? 1.f : 0.f); x < b.x; x += 2.f)
            {
                dl->AddRectFilled(ImVec2(x, y), ImVec2(x + 1.f, y + 1.f), c1);
            }
        }
    }
}
