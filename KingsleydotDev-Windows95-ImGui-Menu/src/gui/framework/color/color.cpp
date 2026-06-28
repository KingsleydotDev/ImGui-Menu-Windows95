#include "color.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "../slider/slider.hpp"
#include "imgui.h"

#include <cstring>
#include <cstdio>
#include <cmath>

namespace ksd
{
    static char s_pickerId[64] = { 0 };
    static ImVec2 s_anchor;
    static bool s_dragSV = false;
    static bool s_dragHue = false;
    static bool s_dragAlpha = false;
    static bool s_dragSpeed = false;
    static bool s_dragMove = false;
    static bool s_xPressed = false;
    static ImVec2 s_moveGrab;

    static constexpr float kScale = 1.25f;
    static constexpr float kSpeedMin = 0.2f;
    static constexpr float kSpeedMax = 4.f;

    static float Clamp01(float x)
    {
        if (x < 0.f) { return 0.f; }
        if (x > 1.f) { return 1.f; }
        return x;
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

    struct RainbowState
    {
        char id[64];
        bool on;
        float speed;
    };

    static RainbowState s_rainbowStore[64];
    static int s_rainbowCount = 0;

    static RainbowState* GetRainbowState(const char* id)
    {
        for (int i = 0; i < s_rainbowCount; ++i)
        {
            if (std::strcmp(s_rainbowStore[i].id, id) == 0)
            {
                return &s_rainbowStore[i];
            }
        }
        if (s_rainbowCount >= 64)
        {
            return &s_rainbowStore[0];
        }
        RainbowState* entry = &s_rainbowStore[s_rainbowCount++];
        CopyStr(entry->id, id, sizeof(entry->id));
        entry->on = false;
        entry->speed = 1.f;
        return entry;
    }

    static ImU32 HsvU32(float h, float s, float v)
    {
        float r, g, b;
        ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
        return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, 1.f));
    }

    static void DrawRainbow(ImDrawList* dl, const ImVec2& a, const ImVec2& b)
    {
        const ImU32 cols[7] = {
            IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255),
            IM_COL32(0, 255, 255, 255), IM_COL32(0, 0, 255, 255), IM_COL32(255, 0, 255, 255),
            IM_COL32(255, 0, 0, 255)
        };
        const float sw = (b.x - a.x) / 6.f;
        for (int i = 0; i < 6; ++i)
        {
            const ImVec2 sa(a.x + sw * i, a.y);
            const ImVec2 sb(a.x + sw * (i + 1), b.y);
            dl->AddRectFilledMultiColor(sa, sb, cols[i], cols[i + 1], cols[i + 1], cols[i]);
        }
    }

    static void DrawHueBar(ImDrawList* dl, const ImVec2& a, const ImVec2& b)
    {
        const ImU32 cols[7] = {
            IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255),
            IM_COL32(0, 255, 255, 255), IM_COL32(0, 0, 255, 255), IM_COL32(255, 0, 255, 255),
            IM_COL32(255, 0, 0, 255)
        };
        const float sh = (b.y - a.y) / 6.f;
        for (int i = 0; i < 6; ++i)
        {
            const ImVec2 sa(a.x, a.y + sh * i);
            const ImVec2 sb(b.x, a.y + sh * (i + 1));
            dl->AddRectFilledMultiColor(sa, sb, cols[i], cols[i], cols[i + 1], cols[i + 1]);
        }
    }

    static void DrawCheckerboard(ImDrawList* dl, const ImVec2& a, const ImVec2& b)
    {
        const float cell = 6.f;
        dl->AddRectFilled(a, b, IM_COL32(170, 170, 170, 255));
        int row = 0;
        for (float y = a.y; y < b.y; y += cell, ++row)
        {
            const float yb = (y + cell < b.y) ? y + cell : b.y;
            for (float x = a.x + ((row & 1) ? cell : 0.f); x < b.x; x += cell * 2.f)
            {
                const float xb = (x + cell < b.x) ? x + cell : b.x;
                dl->AddRectFilled(ImVec2(x, y), ImVec2(xb, yb), IM_COL32(110, 110, 110, 255));
            }
        }
    }

    static void TickRainbow(float col[4], float speed)
    {
        float h, s, v;
        ImGui::ColorConvertRGBtoHSV(col[0], col[1], col[2], h, s, v);
        h = std::fmod(h + ImGui::GetIO().DeltaTime * speed * 0.4f, 1.f);
        if (h < 0.f) { h += 1.f; }
        ImGui::ColorConvertHSVtoRGB(h, s, v, col[0], col[1], col[2]);
    }

    static bool DrawPicker(float col[4], bool* rainbow, float* speed)
    {
        ImDrawList* fg = ImGui::GetForegroundDrawList();
        const float lineH = ImGui::GetTextLineHeight();
        const float pad = 6.f * kScale;
        const float sq = 110.f * kScale;
        const float hueW = 16.f * kScale;
        const float gap = 6.f * kScale;
        const float barH = 14.f * kScale;
        const float previewH = 16.f * kScale;
        const float cbSize = 13.f * kScale;

        const float titleH = 16.f;
        const float topOff = 2.f + titleH;

        const ImVec2 a = s_anchor;
        const ImVec2 svA(a.x + pad, a.y + topOff + pad);
        const ImVec2 svB(svA.x + sq, svA.y + sq);
        const ImVec2 hA(svB.x + gap, svA.y);
        const ImVec2 hB(hA.x + hueW, svB.y);
        const float right = hB.x;

        const ImVec2 alA(svA.x, svB.y + gap);
        const ImVec2 alB(right, alA.y + barH);
        const ImVec2 pvA(svA.x, alB.y + gap);
        const ImVec2 pvB(right, pvA.y + previewH);

        const bool hasRainbow = (rainbow != nullptr);
        const float cbY = pvB.y + gap;
        const bool showSpeed = hasRainbow && *rainbow && (speed != nullptr);
        const float spLabelY = cbY + cbSize + gap;
        const float spGrooveY = spLabelY + lineH + 3.f;

        float bottomY = pvB.y;
        if (hasRainbow)
        {
            bottomY = cbY + cbSize;
            if (showSpeed) { bottomY = spGrooveY + style::SliderTrackH; }
        }
        const ImVec2 b(right + pad, bottomY + pad);

        const ImVec2 tA(a.x + 2.f, a.y + 2.f);
        const ImVec2 tB(b.x - 2.f, a.y + 2.f + titleH);
        const float xBtnW = 14.f;
        const float xBtnH = 12.f;
        const ImVec2 xA(tB.x - xBtnW - 1.f, tA.y + (titleH - xBtnH) * 0.5f);
        const ImVec2 xB(xA.x + xBtnW, xA.y + xBtnH);

        style::FillFace(fg, a, b);
        style::Raised(fg, a, b);

        const ImVec2 m = ImGui::GetMousePos();
        const bool down = ImGui::IsMouseDown(ImGuiMouseButton_Left);
        const bool clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
        const bool xHover = ImGui::IsMouseHoveringRect(xA, xB, false);
        bool changed = false;

        if (clicked)
        {
            s_xPressed = xHover;
            if (ImGui::IsMouseHoveringRect(tA, ImVec2(xA.x, tB.y), false)) { s_dragMove = true; s_moveGrab = ImVec2(m.x - a.x, m.y - a.y); }
            else if (ImGui::IsMouseHoveringRect(svA, svB, false)) { s_dragSV = true; }
            else if (ImGui::IsMouseHoveringRect(hA, hB, false)) { s_dragHue = true; }
            else if (ImGui::IsMouseHoveringRect(alA, alB, false)) { s_dragAlpha = true; }
            else if (showSpeed && ImGui::IsMouseHoveringRect(ImVec2(svA.x, spGrooveY - 2.f), ImVec2(right, spGrooveY + style::SliderTrackH), false)) { s_dragSpeed = true; }
        }
        if (!down) { s_dragSV = s_dragHue = s_dragAlpha = s_dragSpeed = s_dragMove = false; }

        float h, s, v;
        ImGui::ColorConvertRGBtoHSV(col[0], col[1], col[2], h, s, v);

        if (s_dragSV)
        {
            s = Clamp01((m.x - svA.x) / sq);
            v = Clamp01(1.f - (m.y - svA.y) / sq);
            ImGui::ColorConvertHSVtoRGB(h, s, v, col[0], col[1], col[2]);
            changed = true;
        }
        if (s_dragHue)
        {
            h = Clamp01((m.y - hA.y) / (hB.y - hA.y));
            ImGui::ColorConvertHSVtoRGB(h, s, v, col[0], col[1], col[2]);
            changed = true;
        }
        if (s_dragAlpha)
        {
            col[3] = Clamp01((m.x - alA.x) / (alB.x - alA.x));
            changed = true;
        }
        if (s_dragSpeed && speed)
        {
            const float w = right - svA.x;
            const float t = Clamp01((m.x - (svA.x + style::ThumbW * 0.5f)) / (w - style::ThumbW));
            *speed = kSpeedMin + t * (kSpeedMax - kSpeedMin);
            changed = true;
        }

        fg->AddRectFilled(tA, tB, ksd::AccentColor());
        fg->AddText(ImVec2(tA.x + 4.f, tA.y + (titleH - lineH) * 0.5f), style::TitleText, "color");
        const bool xHeld = xHover && down;
        style::FillFace(fg, xA, xB);
        if (xHeld) { style::Pressed(fg, xA, xB); }
        else { style::Raised(fg, xA, xB); }
        const float xp = xHeld ? 1.f : 0.f;
        const ImVec2 xc((xA.x + xB.x) * 0.5f + xp, (xA.y + xB.y) * 0.5f + xp);
        fg->AddLine(ImVec2(xc.x - 3.f, xc.y - 3.f), ImVec2(xc.x + 3.f, xc.y + 3.f), style::Text);
        fg->AddLine(ImVec2(xc.x + 3.f, xc.y - 3.f), ImVec2(xc.x - 3.f, xc.y + 3.f), style::Text);

        const ImU32 hueCol = HsvU32(h, 1.f, 1.f);
        fg->AddRectFilledMultiColor(svA, svB, IM_COL32(255, 255, 255, 255), hueCol, hueCol, IM_COL32(255, 255, 255, 255));
        fg->AddRectFilledMultiColor(svA, svB, IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 255), IM_COL32(0, 0, 0, 255));
        style::Sunken(fg, svA, svB);
        const ImVec2 cur(svA.x + s * sq, svA.y + (1.f - v) * sq);
        fg->AddCircle(cur, 5.f, IM_COL32(255, 255, 255, 255));
        fg->AddCircle(cur, 4.f, IM_COL32(0, 0, 0, 255));

        DrawHueBar(fg, hA, hB);
        style::Sunken(fg, hA, hB);
        const float hy = hA.y + h * (hB.y - hA.y);
        fg->AddRect(ImVec2(hA.x - 2.f, hy - 2.f), ImVec2(hB.x + 2.f, hy + 2.f), IM_COL32(0, 0, 0, 255));

        DrawCheckerboard(fg, alA, alB);
        const int cr = (int)(col[0] * 255.f + 0.5f);
        const int cg = (int)(col[1] * 255.f + 0.5f);
        const int cbl = (int)(col[2] * 255.f + 0.5f);
        fg->AddRectFilledMultiColor(alA, alB, IM_COL32(cr, cg, cbl, 0), IM_COL32(cr, cg, cbl, 255), IM_COL32(cr, cg, cbl, 255), IM_COL32(cr, cg, cbl, 0));
        style::Sunken(fg, alA, alB);
        const float ax = alA.x + col[3] * (alB.x - alA.x);
        style::FillFace(fg, ImVec2(ax - 2.f, alA.y - 2.f), ImVec2(ax + 2.f, alB.y + 2.f));
        style::Raised(fg, ImVec2(ax - 2.f, alA.y - 2.f), ImVec2(ax + 2.f, alB.y + 2.f));

        DrawCheckerboard(fg, pvA, pvB);
        fg->AddRectFilled(pvA, pvB, ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], col[3])));
        style::Sunken(fg, pvA, pvB);

        if (hasRainbow)
        {
            const ImVec2 cbA(svA.x, cbY);
            const ImVec2 cbB(cbA.x + cbSize, cbA.y + cbSize);
            fg->AddRectFilled(cbA, cbB, style::Field);
            style::Sunken(fg, cbA, cbB);
            if (*rainbow)
            {
                fg->AddLine(ImVec2(cbA.x + 4.f, cbA.y + cbSize * 0.5f), ImVec2(cbA.x + cbSize * 0.42f, cbB.y - 4.f), style::Text, 2.f);
                fg->AddLine(ImVec2(cbA.x + cbSize * 0.42f, cbB.y - 4.f), ImVec2(cbB.x - 3.f, cbA.y + 4.f), style::Text, 2.f);
            }
            const char* rlabel = "rainbow";
            const ImVec2 rts = ImGui::CalcTextSize(rlabel);
            fg->AddText(ImVec2(cbB.x + 6.f, cbA.y + (cbSize - lineH) * 0.5f), style::Text, rlabel);
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
                ImGui::IsMouseHoveringRect(cbA, ImVec2(cbB.x + 8.f + rts.x, cbB.y), false))
            {
                *rainbow = !*rainbow;
            }
        }

        if (showSpeed)
        {
            fg->AddText(ImVec2(svA.x, spLabelY), style::Text, "speed");
            char buf[16];
            std::snprintf(buf, sizeof(buf), "%.1fx", *speed);
            const ImVec2 vts = ImGui::CalcTextSize(buf);
            fg->AddText(ImVec2(right - vts.x, spLabelY), style::Text, buf);

            const float st = Clamp01((*speed - kSpeedMin) / (kSpeedMax - kSpeedMin));
            const bool spFocused = s_dragSpeed ||
                ImGui::IsMouseHoveringRect(ImVec2(svA.x, spGrooveY), ImVec2(right, spGrooveY + style::SliderTrackH), false);
            ksd::DrawTrackbar(fg, ImVec2(svA.x, spGrooveY), right - svA.x, st, spFocused);
        }

        if ((s_xPressed && xHover && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) ||
            (clicked && !ImGui::IsMouseHoveringRect(a, b, false)))
        {
            s_pickerId[0] = '\0';
            ksd::SetSelectedItem(nullptr);
            return changed;
        }
        if (!down) { s_xPressed = false; }

        if (s_dragMove && down)
        {
            const ImVec2 disp = ImGui::GetIO().DisplaySize;
            const float pw = b.x - a.x;
            const float ph = b.y - a.y;
            float maxX = disp.x - pw;
            float maxY = disp.y - ph;
            if (maxX < 0.f) { maxX = 0.f; }
            if (maxY < 0.f) { maxY = 0.f; }
            ImVec2 na(m.x - s_moveGrab.x, m.y - s_moveGrab.y);
            if (na.x < 0.f) { na.x = 0.f; }
            if (na.x > maxX) { na.x = maxX; }
            if (na.y < 0.f) { na.y = 0.f; }
            if (na.y > maxY) { na.y = maxY; }
            s_anchor = na;
        }
        return changed;
    }

    bool ColorButton(const char* id, float col[4], const ImVec2& pos, const ImVec2& size, bool* rainbow, float* speed)
    {
        if (rainbow == nullptr || speed == nullptr)
        {
            RainbowState* st = GetRainbowState(id);
            if (rainbow == nullptr) { rainbow = &st->on; }
            if (speed == nullptr) { speed = &st->speed; }
        }

        if (rainbow && *rainbow && speed && !s_dragSV && !s_dragHue && !s_dragAlpha && !s_dragSpeed && !s_dragMove)
        {
            TickRainbow(col, *speed);
        }

        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 a = pos;
        const ImVec2 b(pos.x + size.x, pos.y + size.y);
        const ImVec2 inA(a.x + 1.f, a.y + 1.f);
        const ImVec2 inB(b.x - 1.f, b.y - 1.f);

        if (rainbow && *rainbow)
        {
            DrawRainbow(dl, inA, inB);
        }
        else
        {
            if (col[3] < 0.999f)
            {
                DrawCheckerboard(dl, inA, inB);
            }
            dl->AddRectFilled(inA, inB, ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], col[3])));
        }
        style::Sunken(dl, a, b);

        const bool open = std::strcmp(s_pickerId, id) == 0;
        if (!open && ImGui::IsMouseHoveringRect(a, b) && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ksd::PopUpsAreClosed())
        {
            CopyStr(s_pickerId, id, sizeof(s_pickerId));
            const float pickerW = 190.f;
            const float pickerH = 305.f;
            float ax = b.x - pickerW;
            if (ax < ksd::BodyLeft()) { ax = ksd::BodyLeft(); }
            float ay = b.y + 2.f;
            if (ay + pickerH > ksd::BodyBottom()) { ay = a.y - pickerH; }
            if (ay < ksd::BodyTop()) { ay = ksd::BodyTop(); }
            s_anchor = ImVec2(ax, ay);
            ksd::SetSelectedItem(id);
        }

        bool changed = false;
        if (std::strcmp(s_pickerId, id) == 0)
        {
            changed = DrawPicker(col, rainbow, speed);
        }
        return changed;
    }
}
