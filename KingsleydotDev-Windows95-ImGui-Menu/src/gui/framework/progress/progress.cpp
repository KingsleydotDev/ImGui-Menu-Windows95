#include "progress.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

namespace ksd
{
    void ProgressBar(float fraction)
    {
        if (fraction < 0.f) { fraction = 0.f; }
        if (fraction > 1.f) { fraction = 1.f; }

        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const float w = ksd::ContentWidth();
        const float h = 18.f;
        const ImVec2 a = pos;
        const ImVec2 b(pos.x + w, pos.y + h);

        dl->AddRectFilled(a, b, style::Field);
        style::Sunken(dl, a, b);

        const ImU32 fillCol = ksd::AccentColor();
        const float inset = 3.f;
        const float blockW = 7.f;
        const float gap = 2.f;
        const float top = a.y + inset;
        const float bot = b.y - inset;
        const float fillRight = a.x + inset + (w - inset * 2.f) * fraction;
        float x = a.x + inset;
        while (x + blockW <= fillRight)
        {
            dl->AddRectFilled(ImVec2(x, top), ImVec2(x + blockW, bot), fillCol);
            x += blockW + gap;
        }

        ksd::AddItem(ImVec2(w, h));
    }
}
