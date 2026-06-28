#include "separator.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

namespace ksd
{
    void Separator()
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const float w = ksd::ContentWidth();
        const float y = pos.y + 3.f;
        dl->AddLine(ImVec2(pos.x, y), ImVec2(pos.x + w, y), style::Shadow, 1.f);
        dl->AddLine(ImVec2(pos.x, y + 1.f), ImVec2(pos.x + w, y + 1.f), style::Highlight, 1.f);
        ksd::AddItem(ImVec2(w, 7.f));
    }

    void SeparatorText(const char* label)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const float w = ksd::ContentWidth();
        const ImVec2 ts = ImGui::CalcTextSize(label);

        dl->AddText(pos, style::Text, label);
        const float lineX = pos.x + ts.x + 5.f;
        const float y = pos.y + ts.y * 0.5f;
        dl->AddLine(ImVec2(lineX, y), ImVec2(pos.x + w, y), style::Shadow, 1.f);
        dl->AddLine(ImVec2(lineX, y + 1.f), ImVec2(pos.x + w, y + 1.f), style::Highlight, 1.f);
        ksd::AddItem(ImVec2(w, ts.y));
    }
}
