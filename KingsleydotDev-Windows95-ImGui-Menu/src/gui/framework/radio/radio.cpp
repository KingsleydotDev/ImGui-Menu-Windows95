#include "radio.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

namespace ksd
{
    bool RadioButton(const char* label, int* current, int index)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const float d = style::CheckboxSize;
        const ImVec2 c(pos.x + d * 0.5f, pos.y + d * 0.5f);
        const float rad = d * 0.5f;
        const ImVec2 ts = ImGui::CalcTextSize(label);

        const ImVec2 hitMax(pos.x + d + 5.f + ts.x + 2.f, pos.y + d);
        const bool hovered = ImGui::IsMouseHoveringRect(pos, hitMax) && ksd::PopUpsAreClosed();
        bool changed = false;
        if (hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && *current != index)
        {
            *current = index;
            changed = true;
        }

        dl->AddCircleFilled(c, rad, style::Field, 24);
        dl->AddCircle(c, rad - 0.5f, style::Shadow, 24, 1.f);
        if (*current == index)
        {
            dl->AddCircleFilled(c, rad * 0.4f, style::Text, 12);
        }

        dl->AddText(ImVec2(pos.x + d + 5.f, pos.y + (d - ts.y) * 0.5f), style::Text, label);
        ksd::AddItem(ImVec2(ksd::ContentWidth(), style::RowHeight));
        return changed;
    }
}
