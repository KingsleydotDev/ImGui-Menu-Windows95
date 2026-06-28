#include "spin.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

#include <cstdio>

namespace ksd
{
    static bool SpinArrow(ImDrawList* dl, const ImVec2& a, const ImVec2& b, bool up)
    {
        const bool hovered = ImGui::IsMouseHoveringRect(a, b) && ksd::PopUpsAreClosed();
        const bool held = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
        style::FillFace(dl, a, b);
        if (held)
        {
            style::Pressed(dl, a, b);
        }
        else
        {
            style::Raised(dl, a, b);
        }
        const float push = held ? 1.f : 0.f;
        const ImVec2 c((a.x + b.x) * 0.5f + push, (a.y + b.y) * 0.5f + push);
        if (up)
        {
            dl->AddTriangleFilled(ImVec2(c.x, c.y - 2.f), ImVec2(c.x - 3.f, c.y + 2.f), ImVec2(c.x + 3.f, c.y + 2.f), style::Text);
        }
        else
        {
            dl->AddTriangleFilled(ImVec2(c.x - 3.f, c.y - 2.f), ImVec2(c.x + 3.f, c.y - 2.f), ImVec2(c.x, c.y + 2.f), style::Text);
        }
        return hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    }

    bool SpinInt(const char* label, int* v, int vmin, int vmax, int step)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const float w = ksd::ContentWidth();
        const float h = style::ComboH;
        const float fieldW = 56.f;
        const float btnW = 15.f;

        const ImVec2 ts = ImGui::CalcTextSize(label);
        dl->AddText(ImVec2(pos.x, pos.y + (h - ts.y) * 0.5f), style::Text, label);

        const ImVec2 fa(pos.x + w - fieldW, pos.y);
        const ImVec2 fb(pos.x + w, pos.y + h);
        const ImVec2 boxB(fb.x - btnW, fb.y);
        dl->AddRectFilled(fa, boxB, style::Field);
        style::Sunken(dl, fa, boxB);

        char buf[16];
        std::snprintf(buf, sizeof(buf), "%d", *v);
        const ImVec2 vts = ImGui::CalcTextSize(buf);
        dl->AddText(ImVec2(boxB.x - vts.x - 4.f, pos.y + (h - vts.y) * 0.5f), style::Text, buf);

        const float midY = pos.y + h * 0.5f;
        bool changed = false;
        if (SpinArrow(dl, ImVec2(boxB.x, fa.y), ImVec2(fb.x, midY), true))
        {
            *v += step;
            changed = true;
        }
        if (SpinArrow(dl, ImVec2(boxB.x, midY), ImVec2(fb.x, fb.y), false))
        {
            *v -= step;
            changed = true;
        }

        if (*v < vmin) { *v = vmin; }
        if (*v > vmax) { *v = vmax; }

        ksd::AddItem(ImVec2(w, h));
        return changed;
    }
}
