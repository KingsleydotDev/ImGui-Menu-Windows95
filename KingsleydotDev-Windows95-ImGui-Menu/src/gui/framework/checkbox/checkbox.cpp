#include "checkbox.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "../keybind/keybind.hpp"
#include "../color/color.hpp"
#include "imgui.h"

namespace ksd
{
    static bool DrawCheckbox(const char* label, bool* v)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const ImVec2 boxMin(pos.x, pos.y);
        const ImVec2 boxMax(pos.x + style::CheckboxSize, pos.y + style::CheckboxSize);
        const ImVec2 ts = ImGui::CalcTextSize(label);

        const ImVec2 hitMax(boxMax.x + 5.f + ts.x + 2.f, boxMax.y);
        const bool hovered = ImGui::IsMouseHoveringRect(boxMin, hitMax);
        if (hovered && ksd::PopUpsAreClosed() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            *v = !*v;
        }

        dl->AddRectFilled(boxMin, boxMax, style::Field);
        style::Sunken(dl, boxMin, boxMax);

        if (*v)
        {
            const float x = boxMin.x;
            const float y = boxMin.y;
            dl->AddLine(ImVec2(x + 3.f, y + 6.f), ImVec2(x + 5.f, y + 8.f), style::Text, 1.6f);
            dl->AddLine(ImVec2(x + 5.f, y + 8.f), ImVec2(x + 9.f, y + 3.f), style::Text, 1.6f);
        }

        dl->AddText(ImVec2(boxMax.x + 5.f, boxMin.y + (style::CheckboxSize - ts.y) * 0.5f), style::Text, label);

        ksd::AddItem(ImVec2(ksd::ContentWidth(), style::RowHeight));
        return *v;
    }

    bool Checkbox(const char* label, bool* v)
    {
        return DrawCheckbox(label, v);
    }

    bool CheckboxKeybind(const char* label, bool* on, const char* id, int* key)
    {
        const ImVec2 row = ksd::CursorPos();
        ksd::Checkbox(label, on);
        if (ksd::Keybind(id, key, ImVec2(ksd::PanelRightX() - style::KeybindW, row.y), ImVec2(style::KeybindW, style::KeybindH)))
        {
            *on = !*on;
        }
        return *on;
    }

    bool CheckboxColor(const char* label, bool* on, const char* id, float col[4], bool* rainbow, float* speed)
    {
        const ImVec2 row = ksd::CursorPos();
        ksd::Checkbox(label, on);
        ksd::ColorButton(id, col, ImVec2(ksd::PanelRightX() - style::SwatchW, row.y), ImVec2(style::SwatchW, style::SwatchH), rainbow, speed);
        return *on;
    }
}
