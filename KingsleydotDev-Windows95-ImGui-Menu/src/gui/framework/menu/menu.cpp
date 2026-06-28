#include "menu.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

#include <cstring>

namespace ksd
{
    static float s_barX = 0.f;
    static float s_barY = 0.f;
    static char s_openMenu[64] = { 0 };
    static bool s_openIsContext = false;
    static bool s_anyBarOpen = false;
    static bool s_consumedClick = false;
    static float s_ctxX = 0.f;
    static float s_ctxY = 0.f;

    static bool s_dropActive = false;
    static char s_dropId[64] = { 0 };
    static float s_dropX = 0.f;
    static float s_dropY = 0.f;
    static float s_dropCursorY = 0.f;
    static float s_dropW = 0.f;
    static float s_dropTrackW = 0.f;
    static ImVec2 s_dropMin;
    static ImVec2 s_dropMax;

    static void CopyStr(char* dst, const char* src, size_t cap)
    {
        size_t i = 0;
        for (; src[i] != '\0' && i + 1 < cap; ++i)
        {
            dst[i] = src[i];
        }
        dst[i] = '\0';
    }

    static ImVec2& SizeCache(const char* id)
    {
        static char ids[16][64] = {};
        static ImVec2 sizes[16] = {};
        for (int i = 0; i < 16; ++i)
        {
            if (ids[i][0] == '\0')
            {
                CopyStr(ids[i], id, sizeof(ids[i]));
                sizes[i] = ImVec2(0.f, 0.f);
                return sizes[i];
            }
            if (std::strcmp(ids[i], id) == 0)
            {
                return sizes[i];
            }
        }
        return sizes[0];
    }

    static int StripAmp(const char* label, char* out, int cap)
    {
        int oi = 0;
        int accel = -1;
        for (int i = 0; label[i] != '\0' && oi < cap - 1; ++i)
        {
            if (label[i] == '&' && label[i + 1] != '\0')
            {
                accel = oi;
                continue;
            }
            out[oi++] = label[i];
        }
        out[oi] = '\0';
        return accel;
    }

    static void DrawLabelAccel(ImDrawList* dl, const ImVec2& pos, const char* text, int accel, ImU32 col)
    {
        dl->AddText(pos, col, text);
        if (accel >= 0)
        {
            const float x0 = pos.x + ImGui::CalcTextSize(text, text + accel).x;
            const float x1 = pos.x + ImGui::CalcTextSize(text, text + accel + 1).x;
            const float uy = pos.y + ImGui::GetTextLineHeight() - 1.f;
            dl->AddLine(ImVec2(x0, uy), ImVec2(x1, uy), col, 1.f);
        }
    }

    static void BeginDropdown(float x, float y, const char* id)
    {
        ImDrawList* fg = ImGui::GetForegroundDrawList();
        CopyStr(s_dropId, id, sizeof(s_dropId));

        const ImVec2& cached = SizeCache(id);
        s_dropW = cached.x > 40.f ? cached.x : 130.f;
        const float dropH = cached.y > 10.f ? cached.y : 26.f;

        s_dropMin = ImVec2(x, y);
        s_dropMax = ImVec2(x + s_dropW, y + dropH);
        fg->AddRectFilled(s_dropMin, s_dropMax, style::Face);
        style::Raised(fg, s_dropMin, s_dropMax);

        s_dropActive = true;
        s_dropX = x;
        s_dropY = y;
        s_dropCursorY = y + 3.f;
        s_dropTrackW = 0.f;
    }

    static void EndDropdown()
    {
        ImVec2& cached = SizeCache(s_dropId);
        cached.x = s_dropTrackW;
        cached.y = (s_dropCursorY - s_dropY) + 3.f;
        s_dropActive = false;
    }

    bool BeginMenuBar()
    {
        s_barX = ksd::BodyLeft();
        s_barY = ksd::MenuBarTop();
        s_anyBarOpen = (s_openMenu[0] != '\0') && !s_openIsContext;
        s_consumedClick = false;
        return true;
    }

    void EndMenuBar()
    {
        if (s_anyBarOpen && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !s_consumedClick &&
            !ImGui::IsMouseHoveringRect(s_dropMin, s_dropMax, false))
        {
            s_openMenu[0] = '\0';
        }
    }

    bool BeginMenu(const char* label)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        char stripped[64];
        const int accel = StripAmp(label, stripped, sizeof(stripped));
        const ImVec2 ts = ImGui::CalcTextSize(stripped);
        const float padX = 8.f;
        const float w = ts.x + padX * 2.f;
        const ImVec2 a(s_barX, s_barY);
        const ImVec2 b(s_barX + w, s_barY + style::MenuBarH);

        const bool isOpen = std::strcmp(s_openMenu, label) == 0 && !s_openIsContext;
        const bool hovered = ImGui::IsMouseHoveringRect(a, b);
        const bool lit = isOpen || (hovered && s_anyBarOpen);
        if (lit)
        {
            dl->AddRectFilled(a, b, ksd::AccentColor());
        }
        const ImU32 tc = lit ? style::SelText : style::Text;
        DrawLabelAccel(dl, ImVec2(a.x + padX, a.y + (style::MenuBarH - ts.y) * 0.5f), stripped, accel, tc);

        const bool modalUp = ksd::SelectedItem()[0] != '\0';
        if (hovered && !modalUp && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            s_consumedClick = true;
            if (isOpen)
            {
                s_openMenu[0] = '\0';
            }
            else
            {
                CopyStr(s_openMenu, label, sizeof(s_openMenu));
                s_openIsContext = false;
            }
        }
        else if (hovered && s_anyBarOpen && !isOpen)
        {
            CopyStr(s_openMenu, label, sizeof(s_openMenu));
            s_openIsContext = false;
        }

        s_barX += w;

        const bool nowOpen = std::strcmp(s_openMenu, label) == 0 && !s_openIsContext;
        if (nowOpen)
        {
            BeginDropdown(a.x, b.y, label);
        }
        return nowOpen;
    }

    void EndMenu()
    {
        EndDropdown();
    }

    bool MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
    {
        if (!s_dropActive)
        {
            return false;
        }
        ImDrawList* fg = ImGui::GetForegroundDrawList();
        const float lineH = ImGui::GetTextLineHeight();
        const float itemH = style::RowHeight + 4.f;

        char stripped[64];
        const int accel = StripAmp(label, stripped, sizeof(stripped));

        const ImVec2 ia(s_dropX + 2.f, s_dropCursorY);
        const ImVec2 ib(s_dropX + s_dropW - 2.f, s_dropCursorY + itemH);
        const bool hovered = enabled && ImGui::IsMouseHoveringRect(ia, ib, false);
        if (hovered)
        {
            fg->AddRectFilled(ia, ib, ksd::AccentColor());
        }
        const ImU32 tc = !enabled ? style::Shadow : (hovered ? style::SelText : style::Text);

        const float ty = s_dropCursorY + (itemH - lineH) * 0.5f;
        if (selected)
        {
            const float cx = s_dropX + 6.f;
            const float cy = ty + lineH * 0.5f;
            fg->AddLine(ImVec2(cx, cy), ImVec2(cx + 3.f, cy + 3.f), tc, 1.6f);
            fg->AddLine(ImVec2(cx + 3.f, cy + 3.f), ImVec2(cx + 7.f, cy - 3.f), tc, 1.6f);
        }

        const float textX = s_dropX + 20.f;
        DrawLabelAccel(fg, ImVec2(textX, ty), stripped, accel, tc);
        const float labelW = ImGui::CalcTextSize(stripped).x;

        float shortcutW = 0.f;
        if (shortcut != nullptr)
        {
            shortcutW = ImGui::CalcTextSize(shortcut).x;
            fg->AddText(ImVec2(s_dropX + s_dropW - shortcutW - 10.f, ty), tc, shortcut);
        }

        const float needW = 20.f + labelW + (shortcut != nullptr ? 28.f + shortcutW : 0.f) + 12.f;
        if (needW > s_dropTrackW)
        {
            s_dropTrackW = needW;
        }
        s_dropCursorY += itemH;

        const bool clicked = hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left);
        if (clicked)
        {
            s_consumedClick = true;
            s_openMenu[0] = '\0';
            s_openIsContext = false;
        }
        return clicked;
    }

    void MenuSeparator()
    {
        if (!s_dropActive)
        {
            return;
        }
        ImDrawList* fg = ImGui::GetForegroundDrawList();
        const float y = s_dropCursorY + 3.f;
        fg->AddLine(ImVec2(s_dropX + 3.f, y), ImVec2(s_dropX + s_dropW - 3.f, y), style::Shadow, 1.f);
        fg->AddLine(ImVec2(s_dropX + 3.f, y + 1.f), ImVec2(s_dropX + s_dropW - 3.f, y + 1.f), style::Highlight, 1.f);
        s_dropCursorY += 7.f;
    }

    void OpenPopupMenu(const char* id, float x, float y)
    {
        CopyStr(s_openMenu, id, sizeof(s_openMenu));
        s_openIsContext = true;
        s_ctxX = x;
        s_ctxY = y;
    }

    bool BeginPopupMenu(const char* id)
    {
        if (!s_openIsContext || std::strcmp(s_openMenu, id) != 0)
        {
            return false;
        }
        s_consumedClick = false;
        BeginDropdown(s_ctxX, s_ctxY, id);
        return true;
    }

    bool MenuIsOpen()
    {
        return s_openMenu[0] != '\0';
    }

    void EndPopupMenu()
    {
        EndDropdown();
        const bool clickAway = ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !s_consumedClick &&
            !ImGui::IsMouseHoveringRect(s_dropMin, s_dropMax, false);
        if (clickAway || ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            s_openMenu[0] = '\0';
            s_openIsContext = false;
        }
    }
}
