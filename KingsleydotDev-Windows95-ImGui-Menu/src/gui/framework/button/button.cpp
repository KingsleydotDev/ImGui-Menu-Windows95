#include "button.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

namespace ksd
{
    static bool DrawButton(const char* label, float width)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const ImVec2 ts = ImGui::CalcTextSize(label);

        float w = width;
        if (w <= 0.f)
        {
            w = ts.x + 18.f;
        }
        const ImVec2 a = pos;
        const ImVec2 b(pos.x + w, pos.y + style::ButtonH);

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
        dl->AddText(ImVec2((a.x + b.x - ts.x) * 0.5f + push, (a.y + b.y - ts.y) * 0.5f + push), style::Text, label);

        ksd::AddItem(ImVec2(w, style::ButtonH));
        return hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    }

    bool Button(const char* label, float width)
    {
        return DrawButton(label, width);
    }
}
