#include "label.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

namespace ksd
{
    void Label(const char* text)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const ImVec2 ts = ImGui::CalcTextSize(text);
        dl->AddText(pos, style::Text, text);
        ksd::AddItem(ImVec2(ts.x, style::RowHeight));
    }

    bool Hyperlink(const char* label)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const ImVec2 ts = ImGui::CalcTextSize(label);
        const ImVec2 b(pos.x + ts.x, pos.y + ts.y);

        const bool hovered = ImGui::IsMouseHoveringRect(pos, b) && ksd::PopUpsAreClosed();
        const ImU32 link = IM_COL32(0, 0, 200, 255);
        dl->AddText(pos, link, label);
        dl->AddLine(ImVec2(pos.x, b.y - 1.f), ImVec2(b.x, b.y - 1.f), link, 1.f);

        ksd::AddItem(ImVec2(ts.x, style::RowHeight));
        return hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    }
}
