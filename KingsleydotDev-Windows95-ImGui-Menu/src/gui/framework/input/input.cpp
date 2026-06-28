#include "input.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

#include <cstring>
#include <cmath>

namespace ksd
{
    static char s_activeId[64] = { 0 };
    static int s_cursor = 0;
    static int s_anchor = 0;
    static float s_scrollX = 0.f;
    static float s_blink = 0.f;

    static void CopyStr(char* dst, const char* src, size_t cap)
    {
        size_t i = 0;
        for (; src[i] != '\0' && i + 1 < cap; ++i)
        {
            dst[i] = src[i];
        }
        dst[i] = '\0';
    }

    static float XOfIndex(const char* buf, int i)
    {
        return ImGui::CalcTextSize(buf, buf + i).x;
    }

    static int IndexAtX(const char* buf, int len, float x)
    {
        float acc = 0.f;
        for (int i = 0; i < len; ++i)
        {
            const float cw = ImGui::CalcTextSize(buf + i, buf + i + 1).x;
            if (x < acc + cw * 0.5f)
            {
                return i;
            }
            acc += cw;
        }
        return len;
    }

    static void EraseRange(char* buf, int from, int to)
    {
        const int len = (int)std::strlen(buf);
        if (from < 0) { from = 0; }
        if (to > len) { to = len; }
        if (from >= to) { return; }
        std::memmove(buf + from, buf + to, (size_t)(len - to + 1));
    }

    static void InsertText(char* buf, int bufSize, int pos, const char* ins)
    {
        const int len = (int)std::strlen(buf);
        int il = (int)std::strlen(ins);
        const int avail = bufSize - 1 - len;
        if (il > avail) { il = avail; }
        if (il <= 0) { return; }
        std::memmove(buf + pos + il, buf + pos, (size_t)(len - pos + 1));
        std::memcpy(buf + pos, ins, (size_t)il);
    }

    bool InputText(const char* id, char* buf, int bufSize, float width)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImGuiIO& io = ImGui::GetIO();
        const ImVec2 pos = ksd::CursorPos();
        const float w = (width > 0.f) ? width : ksd::ContentWidth();
        const float h = style::ComboH;
        const float lineH = ImGui::GetTextLineHeight();
        const ImVec2 a = pos;
        const ImVec2 b(pos.x + w, pos.y + h);

        dl->AddRectFilled(a, b, style::Field);
        style::Sunken(dl, a, b);

        const float padX = 4.f;
        const float textX = a.x + padX;
        const float textY = pos.y + (h - lineH) * 0.5f;
        const float fieldW = w - padX * 2.f;
        int len = (int)std::strlen(buf);
        bool changed = false;

        const bool over = ImGui::IsMouseHoveringRect(a, b) && ksd::PopUpsAreClosed();
        const ImVec2 m = ImGui::GetMousePos();

        if (over && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            CopyStr(s_activeId, id, sizeof(s_activeId));
            const int idx = IndexAtX(buf, len, m.x - textX + s_scrollX);
            s_cursor = idx;
            s_anchor = idx;
            s_blink = 0.f;
        }

        bool active = std::strcmp(s_activeId, id) == 0;
        if (active && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !over)
        {
            s_activeId[0] = '\0';
            active = false;
        }

        if (active)
        {
            if (over && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                s_cursor = IndexAtX(buf, len, m.x - textX + s_scrollX);
            }

            const bool ctrl = io.KeyCtrl;
            const bool shift = io.KeyShift;

            for (int n = 0; n < io.InputQueueCharacters.Size; ++n)
            {
                const ImWchar ch = io.InputQueueCharacters[n];
                if (ch >= 32 && ch < 127)
                {
                    if (s_cursor != s_anchor)
                    {
                        const int lo = s_cursor < s_anchor ? s_cursor : s_anchor;
                        const int hi = s_cursor > s_anchor ? s_cursor : s_anchor;
                        EraseRange(buf, lo, hi);
                        s_cursor = lo;
                    }
                    char one[2] = { (char)ch, '\0' };
                    const int before = (int)std::strlen(buf);
                    InsertText(buf, bufSize, s_cursor, one);
                    if ((int)std::strlen(buf) != before)
                    {
                        s_cursor += 1;
                        changed = true;
                    }
                    s_anchor = s_cursor;
                }
            }
            len = (int)std::strlen(buf);

            const bool hasSel = s_cursor != s_anchor;
            const int lo = s_cursor < s_anchor ? s_cursor : s_anchor;
            const int hi = s_cursor > s_anchor ? s_cursor : s_anchor;

            if (ImGui::IsKeyPressed(ImGuiKey_Backspace))
            {
                if (hasSel) { EraseRange(buf, lo, hi); s_cursor = lo; changed = true; }
                else if (s_cursor > 0) { EraseRange(buf, s_cursor - 1, s_cursor); s_cursor -= 1; changed = true; }
                s_anchor = s_cursor;
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_Delete))
            {
                if (hasSel) { EraseRange(buf, lo, hi); s_cursor = lo; changed = true; }
                else if (s_cursor < len) { EraseRange(buf, s_cursor, s_cursor + 1); changed = true; }
                s_anchor = s_cursor;
            }
            else if (ctrl && ImGui::IsKeyPressed(ImGuiKey_A))
            {
                s_anchor = 0;
                s_cursor = len;
            }
            else if (ctrl && (ImGui::IsKeyPressed(ImGuiKey_C) || ImGui::IsKeyPressed(ImGuiKey_X)) && hasSel)
            {
                char tmp[256];
                int n = hi - lo;
                if (n > 255) { n = 255; }
                std::memcpy(tmp, buf + lo, (size_t)n);
                tmp[n] = '\0';
                ImGui::SetClipboardText(tmp);
                if (ImGui::IsKeyPressed(ImGuiKey_X))
                {
                    EraseRange(buf, lo, hi);
                    s_cursor = lo;
                    s_anchor = lo;
                    changed = true;
                }
            }
            else if (ctrl && ImGui::IsKeyPressed(ImGuiKey_V))
            {
                const char* clip = ImGui::GetClipboardText();
                if (clip != nullptr)
                {
                    if (hasSel) { EraseRange(buf, lo, hi); s_cursor = lo; }
                    char clean[256];
                    int ci = 0;
                    for (const char* p = clip; *p != '\0' && ci < 255; ++p)
                    {
                        if (*p >= 32 && *p < 127) { clean[ci++] = *p; }
                    }
                    clean[ci] = '\0';
                    InsertText(buf, bufSize, s_cursor, clean);
                    s_cursor += (int)std::strlen(clean);
                    if (s_cursor > (int)std::strlen(buf)) { s_cursor = (int)std::strlen(buf); }
                    s_anchor = s_cursor;
                    changed = true;
                }
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
            {
                if (s_cursor > 0) { s_cursor -= 1; }
                if (!shift) { s_anchor = s_cursor; }
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
            {
                if (s_cursor < len) { s_cursor += 1; }
                if (!shift) { s_anchor = s_cursor; }
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_Home))
            {
                s_cursor = 0;
                if (!shift) { s_anchor = 0; }
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_End))
            {
                s_cursor = len;
                if (!shift) { s_anchor = len; }
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsKeyPressed(ImGuiKey_Enter))
            {
                s_activeId[0] = '\0';
                active = false;
            }

            len = (int)std::strlen(buf);
            if (s_cursor > len) { s_cursor = len; }
            if (s_anchor > len) { s_anchor = len; }

            const float caretX = XOfIndex(buf, s_cursor);
            if (caretX - s_scrollX > fieldW) { s_scrollX = caretX - fieldW; }
            if (caretX - s_scrollX < 0.f) { s_scrollX = caretX; }
            if (s_scrollX < 0.f) { s_scrollX = 0.f; }
        }

        dl->PushClipRect(ImVec2(textX, a.y + 1.f), ImVec2(b.x - 2.f, b.y - 1.f), true);
        if (active && s_cursor != s_anchor)
        {
            const int lo = s_cursor < s_anchor ? s_cursor : s_anchor;
            const int hi = s_cursor > s_anchor ? s_cursor : s_anchor;
            const float xlo = textX + XOfIndex(buf, lo) - s_scrollX;
            const float xhi = textX + XOfIndex(buf, hi) - s_scrollX;
            dl->AddRectFilled(ImVec2(xlo, textY), ImVec2(xhi, textY + lineH), ksd::AccentColor());
            dl->AddText(ImVec2(textX - s_scrollX, textY), style::Text, buf);
            dl->AddText(ImVec2(xlo, textY), style::SelText, buf + lo, buf + hi);
        }
        else
        {
            dl->AddText(ImVec2(textX - s_scrollX, textY), style::Text, buf);
        }

        if (active)
        {
            s_blink += io.DeltaTime;
            if (std::fmod(s_blink, 1.0f) < 0.5f)
            {
                const float cx = textX + XOfIndex(buf, s_cursor) - s_scrollX;
                dl->AddLine(ImVec2(cx, textY), ImVec2(cx, textY + lineH), style::Text, 1.f);
            }
        }
        dl->PopClipRect();

        ksd::AddItem(ImVec2(w, h));
        return changed;
    }
}
