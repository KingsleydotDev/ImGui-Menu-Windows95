#include "tooltip.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

namespace ksd
{
    void Tooltip(const char* text)
    {
        if (!ksd::IsLastItemHovered())
        {
            return;
        }

        ImDrawList* fg = ImGui::GetForegroundDrawList();
        const ImVec2 m = ImGui::GetMousePos();
        const ImVec2 ts = ImGui::CalcTextSize(text);
        const ImVec2 a(m.x + 14.f, m.y + 18.f);
        const ImVec2 b(a.x + ts.x + 8.f, a.y + ts.y + 4.f);

        fg->AddRectFilled(a, b, IM_COL32(255, 255, 225, 255));
        fg->AddRect(a, b, style::DarkShadow);
        fg->AddText(ImVec2(a.x + 4.f, a.y + 2.f), style::Text, text);
    }
}
