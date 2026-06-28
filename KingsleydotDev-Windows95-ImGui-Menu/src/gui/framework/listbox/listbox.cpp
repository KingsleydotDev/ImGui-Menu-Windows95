#include "listbox.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

#include <cstring>

namespace ksd
{
    static void CopyStr(char* dst, const char* src, size_t cap)
    {
        size_t i = 0;
        for (; src[i] != '\0' && i + 1 < cap; ++i)
        {
            dst[i] = src[i];
        }
        dst[i] = '\0';
    }

    static int& ScrollFor(const char* id)
    {
        static char ids[8][64] = {};
        static int scrolls[8] = {};
        for (int i = 0; i < 8; ++i)
        {
            if (ids[i][0] == '\0')
            {
                CopyStr(ids[i], id, sizeof(ids[i]));
                scrolls[i] = 0;
                return scrolls[i];
            }
            if (std::strcmp(ids[i], id) == 0)
            {
                return scrolls[i];
            }
        }
        return scrolls[0];
    }

    static char s_dragId[64] = {};
    static float s_dragGrab = 0.f;

    static void LbArrow(ImDrawList* dl, const ImVec2& a, const ImVec2& b, bool up, bool held)
    {
        style::FillFace(dl, a, b);
        if (held) { style::Pressed(dl, a, b); }
        else { style::Raised(dl, a, b); }

        const float push = held ? 1.f : 0.f;
        const ImVec2 c((a.x + b.x) * 0.5f + push, (a.y + b.y) * 0.5f + push);
        if (up)
        {
            dl->AddTriangleFilled(ImVec2(c.x, c.y - 2.5f), ImVec2(c.x - 2.5f, c.y + 2.f), ImVec2(c.x + 2.5f, c.y + 2.f), style::Text);
        }
        else
        {
            dl->AddTriangleFilled(ImVec2(c.x - 2.5f, c.y - 2.f), ImVec2(c.x + 2.5f, c.y - 2.f), ImVec2(c.x, c.y + 2.5f), style::Text);
        }
    }

    bool ListBox(const char* id, int* current, const char* const items[], int count, int visibleRows)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const float w = ksd::ContentWidth();
        const float rowH = style::RowHeight + 3.f;
        const float h = rowH * (float)visibleRows + 4.f;
        const ImVec2 a = pos;
        const ImVec2 b(pos.x + w, pos.y + h);

        dl->AddRectFilled(a, b, style::Field);
        style::Sunken(dl, a, b);

        const bool scrollable = count > visibleRows;
        const float sbW = scrollable ? 10.f : 0.f;
        const float listR = b.x - 2.f - sbW;
        const int maxTop = scrollable ? count - visibleRows : 0;
        int& top = ScrollFor(id);

        const bool over = ImGui::IsMouseHoveringRect(a, b) && ksd::PopUpsAreClosed();
        if (over && scrollable)
        {
            const float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.f)
            {
                top -= (int)wheel;
            }
        }
        if (top < 0) { top = 0; }
        if (top > maxTop) { top = maxTop; }

        dl->PushClipRect(ImVec2(a.x + 2.f, a.y + 2.f), ImVec2(listR, b.y - 2.f), true);
        bool changed = false;
        for (int row = 0; row < visibleRows; ++row)
        {
            const int idx = top + row;
            if (idx >= count)
            {
                break;
            }
            const float ry = a.y + 2.f + (float)row * rowH;
            const ImVec2 ra(a.x + 2.f, ry);
            const ImVec2 rb(listR, ry + rowH);
            if (idx == *current)
            {
                dl->AddRectFilled(ra, rb, ksd::AccentColor());
            }
            const ImU32 tc = (idx == *current) ? style::SelText : style::Text;
            dl->AddText(ImVec2(ra.x + 4.f, ry + (rowH - ImGui::GetTextLineHeight()) * 0.5f), tc, items[idx]);
            if (over && ImGui::IsMouseHoveringRect(ra, rb) && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && *current != idx)
            {
                *current = idx;
                changed = true;
            }
        }
        dl->PopClipRect();

        if (scrollable)
        {
            const ImVec2 colMin(listR, a.y + 2.f);
            const ImVec2 colMax(b.x - 2.f, b.y - 2.f);
            const float arrowH = sbW;
            const ImVec2 upA(colMin.x, colMin.y);
            const ImVec2 upB(colMax.x, colMin.y + arrowH);
            const ImVec2 dnA(colMin.x, colMax.y - arrowH);
            const ImVec2 dnB(colMax.x, colMax.y);
            const ImVec2 trA(colMin.x, upB.y);
            const ImVec2 trB(colMax.x, dnA.y);

            style::Stipple(dl, trA, trB, style::Highlight, style::Face);

            const float trackH = trB.y - trA.y;
            float thumbH = trackH * (float)visibleRows / (float)count;
            if (thumbH < 8.f) { thumbH = 8.f; }
            if (thumbH > trackH) { thumbH = trackH; }
            const float thumbRange = trackH - thumbH;
            const float t = (maxTop > 0) ? (float)top / (float)maxTop : 0.f;
            const float thumbY = trA.y + thumbRange * t;
            const ImVec2 thA(trA.x, thumbY);
            const ImVec2 thB(trB.x, thumbY + thumbH);

            const bool down = ImGui::IsMouseDown(ImGuiMouseButton_Left);
            const bool clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
            const bool repeat = ImGui::IsMouseClicked(ImGuiMouseButton_Left, true);
            const bool upHeld = over && down && ImGui::IsMouseHoveringRect(upA, upB);
            const bool dnHeld = over && down && ImGui::IsMouseHoveringRect(dnA, dnB);

            LbArrow(dl, upA, upB, true, upHeld);
            LbArrow(dl, dnA, dnB, false, dnHeld);
            style::FillFace(dl, thA, thB);
            style::Raised(dl, thA, thB);

            const bool dragging = std::strcmp(s_dragId, id) == 0;

            if (over && down && !dragging && ImGui::IsMouseHoveringRect(trA, trB))
            {
                const float my = ImGui::GetMousePos().y;
                if (my < thumbY)
                {
                    style::Stipple(dl, trA, ImVec2(trB.x, thumbY), style::Face, style::Shadow);
                }
                else if (my > thumbY + thumbH)
                {
                    style::Stipple(dl, ImVec2(trA.x, thumbY + thumbH), trB, style::Face, style::Shadow);
                }
            }

            if (dragging)
            {
                if (!down)
                {
                    s_dragId[0] = '\0';
                }
                else if (thumbRange > 0.f)
                {
                    const float my = ImGui::GetMousePos().y;
                    float f = ((my - s_dragGrab) - trA.y) / thumbRange;
                    if (f < 0.f) { f = 0.f; }
                    if (f > 1.f) { f = 1.f; }
                    top = (int)(f * (float)maxTop + 0.5f);
                }
            }
            else if (over)
            {
                if (clicked && ImGui::IsMouseHoveringRect(thA, thB))
                {
                    CopyStr(s_dragId, id, sizeof(s_dragId));
                    s_dragGrab = ImGui::GetMousePos().y - thumbY;
                }
                else if (repeat && ImGui::IsMouseHoveringRect(upA, upB)) { top -= 1; }
                else if (repeat && ImGui::IsMouseHoveringRect(dnA, dnB)) { top += 1; }
                else if (repeat && ImGui::IsMouseHoveringRect(trA, ImVec2(trB.x, thumbY))) { top -= visibleRows; }
                else if (repeat && ImGui::IsMouseHoveringRect(ImVec2(trA.x, thumbY + thumbH), trB)) { top += visibleRows; }
            }

            if (top < 0) { top = 0; }
            if (top > maxTop) { top = maxTop; }
        }

        ksd::AddItem(ImVec2(w, h));
        return changed;
    }
}
