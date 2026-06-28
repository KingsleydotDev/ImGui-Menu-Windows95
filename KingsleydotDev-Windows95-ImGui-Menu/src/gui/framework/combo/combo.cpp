#include "combo.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

#include <cstring>

namespace ksd
{
    static int s_listTop = 0;
    static bool s_listDrag = false;
    static float s_listGrab = 0.f;

    static const int kMaxVisibleItems = 8;

    static void DropArrow(ImDrawList* fg, const ImVec2& a, const ImVec2& b, bool up)
    {
        const bool held = ImGui::IsMouseHoveringRect(a, b) && ImGui::IsMouseDown(ImGuiMouseButton_Left);
        style::FillFace(fg, a, b);
        if (held) { style::Pressed(fg, a, b); }
        else { style::Raised(fg, a, b); }

        const float push = held ? 1.f : 0.f;
        const ImVec2 c((a.x + b.x) * 0.5f + push, (a.y + b.y) * 0.5f + push);
        if (up)
        {
            fg->AddTriangleFilled(ImVec2(c.x, c.y - 3.f), ImVec2(c.x - 3.f, c.y + 2.f), ImVec2(c.x + 3.f, c.y + 2.f), style::Text);
        }
        else
        {
            fg->AddTriangleFilled(ImVec2(c.x - 3.f, c.y - 2.f), ImVec2(c.x + 3.f, c.y - 2.f), ImVec2(c.x, c.y + 3.f), style::Text);
        }
    }

    static int DrawDropdownList(const ImVec2& boxMin, const ImVec2& boxMax,
        const char* const items[], int count, const bool* multiSel, int current,
        bool boxHovered, bool& closeOut)
    {
        ImDrawList* fg = ImGui::GetForegroundDrawList();
        const float lineH = ImGui::GetTextLineHeight();
        const float itemH = style::ComboH - 2.f;
        const bool scroll = count > kMaxVisibleItems;
        const int visible = scroll ? kMaxVisibleItems : count;
        const float listH = itemH * visible + 2.f;

        const float bodyTop = ksd::BodyTop();
        const float bodyBottom = ksd::BodyBottom();
        float listTopY = boxMax.y;
        if (boxMax.y + listH > bodyBottom && boxMin.y - listH >= bodyTop)
        {
            listTopY = boxMin.y - listH;
        }

        const ImVec2 listMin(boxMin.x, listTopY);
        const ImVec2 listMax(boxMax.x, listTopY + listH);
        fg->AddRectFilled(listMin, listMax, style::Field);
        fg->AddRect(listMin, listMax, style::DarkShadow);

        const int maxTop = scroll ? (count - kMaxVisibleItems) : 0;
        if (s_listTop < 0) { s_listTop = 0; }
        if (s_listTop > maxTop) { s_listTop = maxTop; }

        const float sbW = scroll ? style::ScrollbarW : 0.f;
        const float itemsRight = listMax.x - sbW;

        const ImVec2 mp = ImGui::GetMousePos();
        const bool overList = ImGui::IsMouseHoveringRect(listMin, listMax);

        if (scroll && overList)
        {
            const float wheel = ImGui::GetIO().MouseWheel;
            if (wheel > 0.f) { s_listTop -= 1; }
            else if (wheel < 0.f) { s_listTop += 1; }
            if (s_listTop < 0) { s_listTop = 0; }
            if (s_listTop > maxTop) { s_listTop = maxTop; }
        }

        int hovered = -1;
        if (mp.x >= listMin.x && mp.x < itemsRight && mp.y >= listMin.y + 2.f)
        {
            const int row = (int)((mp.y - (listMin.y + 2.f)) / itemH);
            if (row >= 0 && row < visible) { hovered = s_listTop + row; }
        }

        for (int row = 0; row < visible; ++row)
        {
            const int i = s_listTop + row;
            if (i >= count) { break; }
            const ImVec2 iMin(listMin.x + 2.f, listMin.y + 2.f + itemH * row);
            const ImVec2 iMax(itemsRight - 2.f, iMin.y + itemH);

            bool navy = false;
            bool light = false;
            if (multiSel)
            {
                navy = multiSel[i];
                light = (!navy && i == hovered);
            }
            else
            {
                navy = (hovered >= 0) ? (i == hovered) : (i == current);
            }

            if (navy) { fg->AddRectFilled(iMin, iMax, ksd::AccentColor()); }
            else if (light) { fg->AddRectFilled(iMin, iMax, style::Light); }
            fg->AddText(ImVec2(iMin.x + 3.f, iMin.y + (itemH - lineH) * 0.5f), navy ? style::SelText : style::Text, items[i]);
        }

        if (scroll)
        {
            const ImVec2 sbMin(itemsRight, listMin.y + 1.f);
            const ImVec2 sbMax(listMax.x - 1.f, listMax.y - 1.f);
            const float arrowH = 16.f;
            const ImVec2 upMin(sbMin.x, sbMin.y);
            const ImVec2 upMax(sbMax.x, sbMin.y + arrowH);
            const ImVec2 dnMin(sbMin.x, sbMax.y - arrowH);
            const ImVec2 dnMax(sbMax.x, sbMax.y);
            const ImVec2 trMin(sbMin.x, upMax.y);
            const ImVec2 trMax(sbMax.x, dnMin.y);

            fg->AddRectFilled(trMin, trMax, style::Light);
            DropArrow(fg, upMin, upMax, true);
            DropArrow(fg, dnMin, dnMax, false);

            const float trackH = trMax.y - trMin.y;
            float thumbH = trackH * (float)visible / (float)count;
            if (thumbH < 12.f) { thumbH = 12.f; }
            if (thumbH > trackH) { thumbH = trackH; }
            const float thumbRange = trackH - thumbH;
            const float frac = (maxTop > 0) ? (float)s_listTop / (float)maxTop : 0.f;
            const float thumbY = trMin.y + thumbRange * frac;
            const ImVec2 thMin(trMin.x, thumbY);
            const ImVec2 thMax(trMax.x, thumbY + thumbH);
            style::FillFace(fg, thMin, thMax);
            style::Raised(fg, thMin, thMax);

            if (s_listDrag)
            {
                if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                {
                    s_listDrag = false;
                }
                else if (thumbRange > 0.f)
                {
                    float f = ((mp.y - s_listGrab) - trMin.y) / thumbRange;
                    if (f < 0.f) { f = 0.f; }
                    if (f > 1.f) { f = 1.f; }
                    s_listTop = (int)(f * (float)maxTop + 0.5f);
                }
            }
            else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                if (ImGui::IsMouseHoveringRect(thMin, thMax))
                {
                    s_listDrag = true;
                    s_listGrab = mp.y - thumbY;
                }
                else if (ImGui::IsMouseHoveringRect(upMin, upMax)) { s_listTop -= 1; }
                else if (ImGui::IsMouseHoveringRect(dnMin, dnMax)) { s_listTop += 1; }
                if (s_listTop < 0) { s_listTop = 0; }
                if (s_listTop > maxTop) { s_listTop = maxTop; }
            }
        }

        int clicked = -1;
        if (hovered >= 0 && !s_listDrag && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            clicked = hovered;
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !overList && !boxHovered)
        {
            closeOut = true;
        }
        return clicked;
    }

    static void DrawComboField(const char* label, const ImVec2& pos, float w, float lineH,
        ImVec2& boxMinOut, ImVec2& boxMaxOut, ImVec2& arrowMinOut)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();

        float boxY = pos.y;
        if (label[0] != '\0')
        {
            dl->AddText(pos, style::Text, label);
            boxY += lineH + 2.f;
        }

        const ImVec2 boxMin(pos.x, boxY);
        const ImVec2 boxMax(pos.x + w, boxY + style::ComboH);
        dl->AddRectFilled(boxMin, boxMax, style::Field);
        style::Sunken(dl, boxMin, boxMax);

        const ImVec2 arrowMin(boxMax.x - style::ComboH + 2.f, boxMin.y + 2.f);
        const ImVec2 arrowMax(boxMax.x - 2.f, boxMax.y - 2.f);
        style::FillFace(dl, arrowMin, arrowMax);
        style::Raised(dl, arrowMin, arrowMax);
        const ImVec2 ac((arrowMin.x + arrowMax.x) * 0.5f, (arrowMin.y + arrowMax.y) * 0.5f);
        dl->AddTriangleFilled(ImVec2(ac.x - 3.f, ac.y - 1.5f), ImVec2(ac.x + 3.f, ac.y - 1.5f), ImVec2(ac.x, ac.y + 2.5f), style::Text);

        boxMinOut = boxMin;
        boxMaxOut = boxMax;
        arrowMinOut = arrowMin;
    }

    static bool DrawCombo(const char* label, int* current, const char* const items[], int count)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const float w = ksd::ContentWidth();
        const float lineH = ImGui::GetTextLineHeight();

        ImVec2 boxMin, boxMax, arrowMin;
        DrawComboField(label, pos, w, lineH, boxMin, boxMax, arrowMin);

        if (*current >= 0 && *current < count)
        {
            dl->AddText(ImVec2(boxMin.x + 5.f, boxMin.y + (style::ComboH - lineH) * 0.5f), style::Text, items[*current]);
        }

        const bool isOpen = std::strcmp(ksd::SelectedItem(), label) == 0 && label[0] != '\0';
        const bool boxHovered = ImGui::IsMouseHoveringRect(boxMin, boxMax);
        bool changed = false;

        if (boxHovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            if (isOpen) { ksd::SetSelectedItem(nullptr); }
            else if (ksd::PopUpsAreClosed()) { ksd::SetSelectedItem(label); s_listTop = 0; }
        }

        if (isOpen)
        {
            bool close = false;
            const int clicked = DrawDropdownList(boxMin, boxMax, items, count, nullptr, *current, boxHovered, close);
            if (clicked >= 0)
            {
                *current = clicked;
                changed = true;
                ksd::SetSelectedItem(nullptr);
            }
            else if (close)
            {
                ksd::SetSelectedItem(nullptr);
            }
        }

        float used = style::ComboH;
        if (label[0] != '\0') { used += lineH + 2.f; }
        ksd::AddItem(ImVec2(w, used));
        return changed;
    }

    bool Combo(const char* label, int* current, const char* const items[], int count)
    {
        return DrawCombo(label, current, items, count);
    }

    static bool DrawMultiCombo(const char* label, bool* selected, const char* const items[], int count)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const float w = ksd::ContentWidth();
        const float lineH = ImGui::GetTextLineHeight();

        ImVec2 boxMin, boxMax, arrowMin;
        DrawComboField(label, pos, w, lineH, boxMin, boxMax, arrowMin);

        char buf[256];
        int len = 0;
        bool any = false;
        for (int i = 0; i < count; ++i)
        {
            if (!selected[i]) { continue; }
            if (any && len < (int)sizeof(buf) - 3) { buf[len++] = ','; buf[len++] = ' '; }
            for (int k = 0; items[i][k] != '\0' && len < (int)sizeof(buf) - 1; ++k) { buf[len++] = items[i][k]; }
            any = true;
        }
        if (!any)
        {
            const char* none = "none";
            for (int k = 0; none[k] != '\0' && len < (int)sizeof(buf) - 1; ++k) { buf[len++] = none[k]; }
        }
        buf[len] = '\0';

        dl->PushClipRect(ImVec2(boxMin.x + 4.f, boxMin.y), ImVec2(arrowMin.x - 1.f, boxMax.y), true);
        dl->AddText(ImVec2(boxMin.x + 5.f, boxMin.y + (style::ComboH - lineH) * 0.5f), style::Text, buf);
        dl->PopClipRect();

        const bool isOpen = std::strcmp(ksd::SelectedItem(), label) == 0 && label[0] != '\0';
        const bool boxHovered = ImGui::IsMouseHoveringRect(boxMin, boxMax);
        bool changed = false;

        if (boxHovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            if (isOpen) { ksd::SetSelectedItem(nullptr); }
            else if (ksd::PopUpsAreClosed()) { ksd::SetSelectedItem(label); s_listTop = 0; }
        }

        if (isOpen)
        {
            bool close = false;
            const int clicked = DrawDropdownList(boxMin, boxMax, items, count, selected, -1, boxHovered, close);
            if (clicked >= 0)
            {
                selected[clicked] = !selected[clicked];
                changed = true;
            }
            else if (close)
            {
                ksd::SetSelectedItem(nullptr);
            }
        }

        float used = style::ComboH;
        if (label[0] != '\0') { used += lineH + 2.f; }
        ksd::AddItem(ImVec2(w, used));
        return changed;
    }

    bool MultiCombo(const char* label, bool* selected, const char* const items[], int count)
    {
        return DrawMultiCombo(label, selected, items, count);
    }
}
