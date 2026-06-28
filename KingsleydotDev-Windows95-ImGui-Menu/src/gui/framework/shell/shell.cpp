#include "shell.hpp"
#include "../styling.hpp"
#include "../images/images.hpp"
#include "../menu/menu.hpp"
#include "imgui.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <cstring>
#include <cfloat>

#pragma comment(lib, "shell32.lib")

namespace ksd
{
    static HWND s_hwnd = nullptr;
    static ImDrawList* s_dl = nullptr;
    static HostMode s_mode = HostMode::Standalone;
    static bool s_forcePlace = false;
    static bool s_closeRequested = false;

    static ImVec2 s_winMin;
    static ImVec2 s_winMax;
    static ImVec2 s_contentMin;
    static ImVec2 s_contentMax;

    static int s_columns = 2;
    static float s_colX[4] = { 0 };
    static float s_colW[4] = { 0 };
    static float s_colY[4] = { 0 };

    static int s_curCol = 0;
    static float s_panelX = 0.f;
    static float s_panelW = 0.f;
    static float s_indent = 0.f;

    static ImU32 s_accent = style::TitleLeft;
    static bool s_accentOn = false;
    static float s_groupStartY = 0.f;
    static char s_groupLabel[64] = { 0 };
    static ImVec2 s_cursor;
    static ImVec2 s_lastItemMin;
    static ImVec2 s_lastItemMax;

    static float s_tabX = 0.f;
    static float s_tabY = 0.f;
    static float s_bodyTop = 0.f;
    static float s_menuBarY = 0.f;
    static float s_toolbarY = 0.f;
    static bool s_toolbarVisible = true;
    static int s_currentTab = 0;

    static char s_popup[64] = { 0 };
    static bool s_popupBlockClicks = false;

    static bool s_dragging = false;
    static bool s_resizing = false;
    static ImVec2 s_dragOffset;

    static float s_scrollY = 0.f;
    static float s_maxColBottom = 0.f;
    static bool s_needScrollbar = false;
    static ImVec2 s_bodyMin;
    static ImVec2 s_bodyMax;
    static bool s_contentClipped = false;
    static bool s_scrollDrag = false;
    static float s_scrollGrab = 0.f;

    static constexpr int kMinWidth = 390;
    static constexpr int kMinHeight = 292;

    static void MoveWindowTo(int x, int y)
    {
        if (s_hwnd)
        {
            ::SetWindowPos(s_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    static void ResizeWindowTo(int w, int h)
    {
        if (s_hwnd)
        {
            ::SetWindowPos(s_hwnd, nullptr, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    static void CopyStr(char* dst, const char* src, size_t cap)
    {
        size_t i = 0;
        for (; src[i] != '\0' && i + 1 < cap; ++i)
        {
            dst[i] = src[i];
        }
        dst[i] = '\0';
    }

    static void DrawTitleGlyph(const ImVec2& c, int glyph, ImU32 col)
    {
        if (glyph == 0)
        {
            s_dl->AddRectFilled(ImVec2(c.x - 4.f, c.y + 3.f), ImVec2(c.x + 2.f, c.y + 5.f), col);
        }
        else if (glyph == 1)
        {
            s_dl->AddRect(ImVec2(c.x - 4.f, c.y - 4.f), ImVec2(c.x + 4.f, c.y + 4.f), col);
            s_dl->AddRectFilled(ImVec2(c.x - 4.f, c.y - 4.f), ImVec2(c.x + 4.f, c.y - 2.f), col);
        }
        else
        {
            for (float t = -3.f; t <= 3.f; t += 1.f)
            {
                s_dl->AddRectFilled(ImVec2(c.x + t - 1.f, c.y + t - 1.f), ImVec2(c.x + t + 1.f, c.y + t + 1.f), col);
                s_dl->AddRectFilled(ImVec2(c.x + t - 1.f, c.y - t - 1.f), ImVec2(c.x + t + 1.f, c.y - t + 1.f), col);
            }
        }
    }

    static bool TitleButton(const ImVec2& a, const ImVec2& b, int glyph, bool enabled = true)
    {
        const bool hovered = enabled && ImGui::IsMouseHoveringRect(a, b);
        const bool held = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

        style::FillFace(s_dl, a, b);
        if (held)
        {
            style::Pressed(s_dl, a, b);
        }
        else
        {
            style::Raised(s_dl, a, b);
        }

        const float push = held ? 1.f : 0.f;
        const ImVec2 c((a.x + b.x) * 0.5f + push, (a.y + b.y) * 0.5f + push);

        if (enabled)
        {
            DrawTitleGlyph(c, glyph, style::Text);
        }
        else
        {
            DrawTitleGlyph(ImVec2(c.x + 1.f, c.y + 1.f), glyph, style::Highlight);
            DrawTitleGlyph(c, glyph, style::Shadow);
        }

        return enabled && hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    }

    static void UpdateDrag(const ImVec2& captionMin, const ImVec2& captionMax)
    {
        if (s_dragging)
        {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                s_dragging = false;
                return;
            }
            if (s_mode == HostMode::Standalone)
            {
                POINT p;
                ::GetCursorPos(&p);
                MoveWindowTo((int)(p.x - s_dragOffset.x), (int)(p.y - s_dragOffset.y));
            }
            else
            {
                const ImVec2 m = ImGui::GetMousePos();
                ImGui::SetWindowPos("##win95", ImVec2(m.x - s_dragOffset.x, m.y - s_dragOffset.y));
            }
            return;
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(captionMin, captionMax))
        {
            if (s_mode == HostMode::Standalone)
            {
                POINT p;
                ::GetCursorPos(&p);
                RECT r;
                ::GetWindowRect(s_hwnd, &r);
                s_dragOffset = ImVec2((float)(p.x - r.left), (float)(p.y - r.top));
            }
            else
            {
                const ImVec2 m = ImGui::GetMousePos();
                s_dragOffset = ImVec2(m.x - s_winMin.x, m.y - s_winMin.y);
            }
            s_dragging = true;
        }
    }

    static void UpdateResize(const ImVec2& gripMin, const ImVec2& gripMax)
    {
        if (s_resizing)
        {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                s_resizing = false;
                return;
            }
            if (s_mode == HostMode::Standalone)
            {
                POINT p;
                ::GetCursorPos(&p);
                RECT r;
                ::GetWindowRect(s_hwnd, &r);
                int w = (int)p.x - r.left;
                int h = (int)p.y - r.top;
                if (w < kMinWidth) { w = kMinWidth; }
                if (h < kMinHeight) { h = kMinHeight; }
                ResizeWindowTo(w, h);
            }
            else
            {
                const ImVec2 m = ImGui::GetMousePos();
                float w = m.x - s_winMin.x;
                float h = m.y - s_winMin.y;
                if (w < (float)kMinWidth) { w = (float)kMinWidth; }
                if (h < (float)kMinHeight) { h = (float)kMinHeight; }
                ImGui::SetWindowSize("##win95", ImVec2(w, h));
            }
            return;
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(gripMin, gripMax))
        {
            s_resizing = true;
        }
    }

    static void SetupColumns()
    {
        const float totalW = s_contentMax.x - s_contentMin.x;
        const float gw = (totalW - style::ColumnGutter * (s_columns - 1)) / (float)s_columns;
        for (int i = 0; i < s_columns; ++i)
        {
            s_colX[i] = s_contentMin.x + (float)i * (gw + style::ColumnGutter);
            s_colW[i] = gw;
            s_colY[i] = s_contentMin.y + 4.f - s_scrollY;
        }
    }

    void SetWindowHandle(void* hwnd)
    {
        s_hwnd = (HWND)hwnd;
    }

    void SetHostMode(HostMode mode)
    {
        s_mode = mode;
        if (mode == HostMode::Embedded)
        {
            s_forcePlace = true;
        }
    }

    HostMode GetHostMode()
    {
        return s_mode;
    }

    bool CloseRequested()
    {
        const bool r = s_closeRequested;
        s_closeRequested = false;
        return r;
    }

    void SetColumns(int count)
    {
        if (count < 1) { count = 1; }
        if (count > 4) { count = 4; }
        s_columns = count;
        SetupColumns();
    }

    int& CurrentTab()
    {
        return s_currentTab;
    }

    void BeginMain(const char* title)
    {
        const ImGuiIO& io = ImGui::GetIO();
        const ImVec2 size = io.DisplaySize;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, style::Face);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;

        if (s_mode == HostMode::Standalone)
        {
            ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
            ImGui::SetNextWindowSize(size);
            flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            const ImVec2 def(457.f, 423.f);
            const ImGuiCond cond = s_forcePlace ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
            ImGui::SetNextWindowPos(ImVec2((size.x - def.x) * 0.5f, (size.y - def.y) * 0.5f), cond);
            ImGui::SetNextWindowSize(def, cond);
            ImGui::SetNextWindowSizeConstraints(ImVec2((float)kMinWidth, (float)kMinHeight), ImVec2(FLT_MAX, FLT_MAX));
            s_forcePlace = false;
        }

        ImGui::Begin("##win95", nullptr, flags);
        s_dl = ImGui::GetWindowDrawList();
        s_winMin = ImGui::GetWindowPos();
        const ImVec2 winSize = ImGui::GetWindowSize();
        s_winMax = ImVec2(s_winMin.x + winSize.x, s_winMin.y + winSize.y);

        style::Raised(s_dl, s_winMin, s_winMax);

        const float inset = style::FrameBorder;
        const ImVec2 titleMin(s_winMin.x + inset, s_winMin.y + inset);
        const ImVec2 titleMax(s_winMax.x - inset, titleMin.y + style::TitleBarH);
        s_dl->AddRectFilled(titleMin, titleMax, AccentColor());

        const float icoSize = style::TitleBarH - 4.f;
        const ImVec2 icoMin(titleMin.x + 3.f, titleMin.y + (style::TitleBarH - icoSize) * 0.5f);
        const ImVec2 icoMax(icoMin.x + icoSize, icoMin.y + icoSize);
        if (images::Icon() != 0)
        {
            s_dl->AddImage(images::Icon(), icoMin, icoMax);
        }
        else
        {
            style::FillFace(s_dl, icoMin, icoMax);
            style::Raised(s_dl, icoMin, icoMax);
        }

        const float textY = titleMin.y + (style::TitleBarH - ImGui::GetTextLineHeight()) * 0.5f;
        s_dl->AddText(ImVec2(icoMax.x + 4.f, textY), style::TitleText, title);

        const float btnY = titleMin.y + (style::TitleBarH - style::TitleBtnH) * 0.5f;
        const float closeX = titleMax.x - style::TitleBtnW - 1.f;
        const float maxX = closeX - style::TitleBtnW;
        const float minX = maxX - style::TitleBtnW - 1.f;

        if (TitleButton(ImVec2(closeX, btnY), ImVec2(closeX + style::TitleBtnW, btnY + style::TitleBtnH), 2))
        {
            if (s_mode == HostMode::Standalone) { ::PostMessageW(s_hwnd, WM_CLOSE, 0, 0); }
            else { s_closeRequested = true; }
        }
        TitleButton(ImVec2(maxX, btnY), ImVec2(maxX + style::TitleBtnW, btnY + style::TitleBtnH), 1, false);
        if (TitleButton(ImVec2(minX, btnY), ImVec2(minX + style::TitleBtnW, btnY + style::TitleBtnH), 0))
        {
            s_closeRequested = true;
        }

        UpdateDrag(titleMin, ImVec2(minX, titleMax.y));

        s_menuBarY = titleMax.y;
        s_toolbarY = s_menuBarY + style::MenuBarH;

        s_tabY = s_toolbarY + (s_toolbarVisible ? style::ToolbarH : 0.f) + 2.f;
        s_bodyTop = s_tabY + style::TabBarH;
        s_tabX = s_winMin.x + inset + 4.f;

        const ImVec2 bodyMin(s_winMin.x + inset, s_bodyTop);
        const ImVec2 bodyMax(s_winMax.x - inset, s_winMax.y - inset - style::StatusBarH - 2.f);
        style::Raised(s_dl, bodyMin, bodyMax);
        s_bodyMin = bodyMin;
        s_bodyMax = bodyMax;

        s_contentMin = ImVec2(bodyMin.x + style::ContentPad, bodyMin.y + style::ContentPad);
        s_contentMax = ImVec2(bodyMax.x - style::ContentPad, bodyMax.y - style::ContentPad);
        if (s_needScrollbar)
        {
            s_contentMax.x -= style::ScrollbarW;
        }
        SetupColumns();

        s_maxColBottom = s_contentMin.y - s_scrollY;
        s_contentClipped = false;
        s_popupBlockClicks = (s_popup[0] != '\0') || MenuIsOpen();
    }

    bool TabButton(const char* label, int index)
    {
        const bool active = (s_currentTab == index);
        const ImVec2 ts = ImGui::CalcTextSize(label);
        const float w = ts.x + style::TabPadX * 2.f;

        const float top = active ? s_tabY : s_tabY + 2.f;
        const ImVec2 a(s_tabX, top);
        const ImVec2 b(s_tabX + w, s_bodyTop + (active ? 1.f : -1.f));

        style::FillFace(s_dl, a, b);
        style::Edge(s_dl, a, b, style::Highlight, style::Shadow);
        s_dl->AddRectFilled(ImVec2(b.x - 1.f, a.y), ImVec2(b.x, b.y), style::DarkShadow);

        if (active)
        {
            s_dl->AddRectFilled(ImVec2(a.x + 1.f, s_bodyTop - 1.f), ImVec2(b.x - 1.f, s_bodyTop + 2.f), style::Face);
        }

        const ImVec2 tp(a.x + style::TabPadX, (a.y + b.y) * 0.5f - ts.y * 0.5f);
        s_dl->AddText(tp, style::Text, label);

        bool clicked = false;
        if (ImGui::IsMouseHoveringRect(a, b) && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && PopUpsAreClosed())
        {
            if (s_currentTab != index) { s_scrollY = 0.f; }
            s_currentTab = index;
            clicked = true;
        }

        s_tabX = b.x + 2.f;
        return clicked;
    }

    bool BeginGroup(const char* label, int column)
    {
        if (column < 0) { column = 0; }
        if (column >= s_columns) { column = s_columns - 1; }

        if (!s_contentClipped)
        {
            s_dl->PushClipRect(ImVec2(s_bodyMin.x + 1.f, s_bodyTop + 1.f),
                ImVec2(s_bodyMax.x - 1.f, s_bodyMax.y - 1.f), true);
            s_contentClipped = true;
        }

        s_curCol = column;
        s_panelX = s_colX[column];
        s_panelW = s_colW[column];
        s_groupStartY = s_colY[column];
        s_cursor = ImVec2(s_panelX + style::GroupInnerX, s_groupStartY + style::GroupTopPad);
        s_indent = 0.f;

        CopyStr(s_groupLabel, label, sizeof(s_groupLabel));
        return true;
    }

    void EndGroup()
    {
        const float bottom = s_cursor.y - style::RowSpacing + style::GroupBottomPad;
        const float h = bottom - s_groupStartY;
        const ImVec2 a(s_panelX, s_groupStartY);
        const ImVec2 b(s_panelX + s_panelW, s_groupStartY + h);

        style::Etched(s_dl, a, b);

        const ImVec2 ts = ImGui::CalcTextSize(s_groupLabel);
        const float lx = a.x + 7.f;
        s_dl->AddRectFilled(ImVec2(lx - 2.f, a.y - 1.f), ImVec2(lx + ts.x + 2.f, a.y + 2.f), style::Face);
        s_dl->AddText(ImVec2(lx, a.y - ts.y * 0.5f + 1.f), style::Text, s_groupLabel);

        s_colY[s_curCol] = s_groupStartY + h + style::GroupGap;
        if (s_colY[s_curCol] > s_maxColBottom)
        {
            s_maxColBottom = s_colY[s_curCol];
        }
    }

    ImVec2 CursorPos()
    {
        return s_cursor;
    }

    void AddItem(const ImVec2& size)
    {
        s_lastItemMin = s_cursor;
        s_lastItemMax = ImVec2(s_cursor.x + size.x, s_cursor.y + size.y);
        s_cursor.y += size.y + style::RowSpacing;
    }

    bool IsLastItemHovered()
    {
        return PopUpsAreClosed() && ImGui::IsMouseHoveringRect(s_lastItemMin, s_lastItemMax);
    }

    void Indent(float w)
    {
        s_cursor.x += w;
        s_indent += w;
    }

    void Unindent(float w)
    {
        s_cursor.x -= w;
        s_indent -= w;
    }

    float ContentWidth()
    {
        return s_panelW - style::GroupInnerX * 2.f - s_indent;
    }

    float PanelLeftX()
    {
        return s_panelX + style::GroupInnerX;
    }

    float PanelRightX()
    {
        return s_panelX + s_panelW - style::GroupInnerX;
    }

    float BodyTop()
    {
        return s_bodyTop;
    }

    float BodyBottom()
    {
        return s_bodyMax.y;
    }

    float MenuBarTop()
    {
        return s_menuBarY;
    }

    float ToolbarTop()
    {
        return s_toolbarY;
    }

    void SetToolbarVisible(bool visible)
    {
        s_toolbarVisible = visible;
    }

    void SetAccent(const float col[4], bool enabled)
    {
        s_accentOn = enabled;
        s_accent = ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 1.f));
    }

    ImU32 AccentColor()
    {
        if (s_accentOn)
        {
            return s_accent;
        }
        return style::Selection;
    }

    float BodyLeft()
    {
        return s_bodyMin.x;
    }

    float BodyRight()
    {
        return s_bodyMax.x;
    }

    bool PopUpsAreClosed()
    {
        return s_popup[0] == '\0' && !s_popupBlockClicks;
    }

    const char* SelectedItem()
    {
        return s_popup;
    }

    void SetSelectedItem(const char* id)
    {
        if (!id)
        {
            s_popup[0] = '\0';
            return;
        }
        CopyStr(s_popup, id, sizeof(s_popup));
    }

    void OpenUrl(const char* url)
    {
        ShellExecuteA(nullptr, "open", url, nullptr, nullptr, SW_SHOWNORMAL);
    }

    static void DrawGrip(const ImVec2& corner)
    {
        for (int i = 0; i < 3; ++i)
        {
            const float o = (float)(i * 4);
            const ImVec2 a(corner.x - 2.f - o, corner.y - 2.f);
            const ImVec2 b(corner.x - 2.f, corner.y - 2.f - o);
            s_dl->AddLine(ImVec2(a.x, a.y + 1.f), ImVec2(b.x + 1.f, b.y + 1.f), style::Highlight, 1.f);
            s_dl->AddLine(a, b, style::Shadow, 1.f);
        }
    }

    static void ScrollArrow(const ImVec2& a, const ImVec2& b, bool up)
    {
        const bool held = ImGui::IsMouseHoveringRect(a, b) && ImGui::IsMouseDown(ImGuiMouseButton_Left);
        style::FillFace(s_dl, a, b);
        if (held) { style::Pressed(s_dl, a, b); }
        else { style::Raised(s_dl, a, b); }

        const float push = held ? 1.f : 0.f;
        const ImVec2 c((a.x + b.x) * 0.5f + push, (a.y + b.y) * 0.5f + push);
        if (up)
        {
            s_dl->AddTriangleFilled(ImVec2(c.x, c.y - 3.f), ImVec2(c.x - 3.f, c.y + 2.f), ImVec2(c.x + 3.f, c.y + 2.f), style::Text);
        }
        else
        {
            s_dl->AddTriangleFilled(ImVec2(c.x - 3.f, c.y - 2.f), ImVec2(c.x + 3.f, c.y - 2.f), ImVec2(c.x, c.y + 3.f), style::Text);
        }
    }

    static void DrawScrollbar(float availH, float contentHeight, float maxScroll)
    {
        const ImVec2 sbMin(s_bodyMax.x - 2.f - style::ScrollbarW, s_bodyTop + 2.f);
        const ImVec2 sbMax(s_bodyMax.x - 2.f, s_bodyMax.y - 2.f);

        const ImVec2 upMin(sbMin.x, sbMin.y);
        const ImVec2 upMax(sbMax.x, sbMin.y + style::ScrollArrowBtn);
        const ImVec2 dnMin(sbMin.x, sbMax.y - style::ScrollArrowBtn);
        const ImVec2 dnMax(sbMax.x, sbMax.y);
        const ImVec2 trMin(sbMin.x, upMax.y);
        const ImVec2 trMax(sbMax.x, dnMin.y);

        style::Stipple(s_dl, trMin, trMax, style::Highlight, style::Face);
        ScrollArrow(upMin, upMax, true);
        ScrollArrow(dnMin, dnMax, false);

        const float trackH = trMax.y - trMin.y;
        float thumbH = (contentHeight > 0.f) ? trackH * availH / contentHeight : trackH;
        if (thumbH < style::ScrollMinThumb) { thumbH = style::ScrollMinThumb; }
        if (thumbH > trackH) { thumbH = trackH; }
        const float thumbRange = trackH - thumbH;
        const float frac = (maxScroll > 0.f) ? (s_scrollY / maxScroll) : 0.f;
        const float thumbY = trMin.y + thumbRange * frac;
        const ImVec2 thMin(trMin.x, thumbY);
        const ImVec2 thMax(trMax.x, thumbY + thumbH);
        style::FillFace(s_dl, thMin, thMax);
        style::Raised(s_dl, thMin, thMax);

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !s_scrollDrag && PopUpsAreClosed() &&
            ImGui::IsMouseHoveringRect(trMin, trMax))
        {
            const float my = ImGui::GetMousePos().y;
            if (my < thumbY)
            {
                style::Stipple(s_dl, trMin, ImVec2(trMax.x, thumbY), style::Face, style::Shadow);
            }
            else if (my > thumbY + thumbH)
            {
                style::Stipple(s_dl, ImVec2(trMin.x, thumbY + thumbH), trMax, style::Face, style::Shadow);
            }
        }

        if (s_scrollDrag)
        {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                s_scrollDrag = false;
            }
            else if (thumbRange > 0.f)
            {
                const float my = ImGui::GetMousePos().y;
                float f = ((my - s_scrollGrab) - trMin.y) / thumbRange;
                if (f < 0.f) { f = 0.f; }
                if (f > 1.f) { f = 1.f; }
                s_scrollY = f * maxScroll;
            }
            return;
        }

        if (!PopUpsAreClosed())
        {
            return;
        }

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            if (ImGui::IsMouseHoveringRect(upMin, upMax)) { s_scrollY -= 4.f; }
            else if (ImGui::IsMouseHoveringRect(dnMin, dnMax)) { s_scrollY += 4.f; }
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(thMin, thMax))
        {
            s_scrollDrag = true;
            s_scrollGrab = ImGui::GetMousePos().y - thumbY;
        }
        else if (ImGui::IsMouseHoveringRect(trMin, trMax) && ImGui::IsMouseClicked(ImGuiMouseButton_Left, true))
        {
            const float my = ImGui::GetMousePos().y;
            if (my < thumbY) { s_scrollY -= availH * 0.9f; }
            else if (my > thumbY + thumbH) { s_scrollY += availH * 0.9f; }
        }
    }

    void EndMain()
    {
        if (s_contentClipped)
        {
            s_dl->PopClipRect();
            s_contentClipped = false;
        }

        const float availH = s_contentMax.y - s_contentMin.y;
        const float contentHeight = (s_maxColBottom + s_scrollY) - s_contentMin.y;
        const float maxScroll = (contentHeight > availH) ? (contentHeight - availH) : 0.f;
        s_needScrollbar = maxScroll > 0.f;

        if (s_needScrollbar)
        {
            if (ImGui::IsMouseHoveringRect(s_bodyMin, s_bodyMax) && PopUpsAreClosed())
            {
                const float wheel = ImGui::GetIO().MouseWheel;
                if (wheel != 0.f) { s_scrollY -= wheel * 40.f; }
            }
            if (s_scrollY < 0.f) { s_scrollY = 0.f; }
            if (s_scrollY > maxScroll) { s_scrollY = maxScroll; }
            DrawScrollbar(availH, contentHeight, maxScroll);
        }
        else
        {
            s_scrollY = 0.f;
        }

        if (s_scrollY < 0.f) { s_scrollY = 0.f; }
        if (s_scrollY > maxScroll) { s_scrollY = maxScroll; }

        const float inset = style::FrameBorder;
        const ImVec2 statusMin(s_winMin.x + inset, s_winMax.y - inset - style::StatusBarH);
        const ImVec2 statusMax(s_winMax.x - inset, s_winMax.y - inset);

        const ImVec2 cellMax(statusMax.x - 16.f, statusMax.y);
        style::Sunken(s_dl, statusMin, cellMax);
        const float ty = statusMin.y + (style::StatusBarH - ImGui::GetTextLineHeight()) * 0.5f;
        s_dl->AddText(ImVec2(statusMin.x + 5.f, ty), style::Text, "Kingsley");
        const char* uid = "UID: 1337";
        const ImVec2 uidSize = ImGui::CalcTextSize(uid);
        s_dl->AddText(ImVec2(cellMax.x - uidSize.x - 5.f, ty), style::Text, uid);

        DrawGrip(ImVec2(statusMax.x, statusMax.y));
        UpdateResize(ImVec2(statusMax.x - style::GripSize, statusMax.y - style::GripSize), statusMax);

        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
}
