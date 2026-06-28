#include "toolbar.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

namespace ksd
{
    static float s_tbX = 0.f;
    static float s_tbY = 0.f;

    static void Glyph(ImDrawList* dl, const ImVec2& c, int glyph, bool on)
    {
        const ImU32 line = on ? style::DarkShadow : style::Shadow;
        const ImU32 white = IM_COL32(255, 255, 255, 255);

        if (glyph == 0)
        {
            const ImVec2 a(c.x - 5.f, c.y - 6.f);
            const ImVec2 b(c.x + 4.f, c.y + 6.f);
            dl->AddRectFilled(a, b, white);
            dl->AddRect(a, b, line);
            dl->AddTriangleFilled(ImVec2(b.x - 3.f, a.y), ImVec2(b.x, a.y + 3.f), ImVec2(b.x - 3.f, a.y + 3.f), IM_COL32(190, 190, 190, 255));
            for (int i = 0; i < 3; ++i)
            {
                dl->AddLine(ImVec2(a.x + 2.f, a.y + 5.f + i * 3.f), ImVec2(b.x - 2.f, a.y + 5.f + i * 3.f), style::Shadow, 1.f);
            }
        }
        else if (glyph == 1)
        {
            const ImU32 fold = on ? IM_COL32(240, 200, 90, 255) : style::Shadow;
            const ImVec2 a(c.x - 6.f, c.y - 3.f);
            const ImVec2 b(c.x + 6.f, c.y + 5.f);
            dl->AddRectFilled(ImVec2(a.x, a.y - 3.f), ImVec2(a.x + 6.f, a.y + 1.f), fold);
            dl->AddRectFilled(a, b, fold);
            dl->AddRect(a, b, line);
        }
        else if (glyph == 2)
        {
            const ImU32 navy = on ? IM_COL32(40, 60, 140, 255) : style::Shadow;
            const ImVec2 a(c.x - 6.f, c.y - 6.f);
            const ImVec2 b(c.x + 6.f, c.y + 6.f);
            dl->AddRectFilled(a, b, navy);
            dl->AddRect(a, b, line);
            dl->AddRectFilled(ImVec2(a.x + 2.f, c.y + 1.f), ImVec2(b.x - 2.f, b.y - 1.f), white);
            dl->AddRectFilled(ImVec2(c.x, a.y + 1.f), ImVec2(b.x - 2.f, c.y - 1.f), IM_COL32(170, 170, 170, 255));
        }
        else if (glyph == 3)
        {
            dl->AddCircle(ImVec2(c.x - 3.f, c.y + 3.f), 2.5f, line, 10, 1.f);
            dl->AddCircle(ImVec2(c.x + 3.f, c.y + 3.f), 2.5f, line, 10, 1.f);
            dl->AddLine(ImVec2(c.x - 3.f, c.y + 2.f), ImVec2(c.x + 5.f, c.y - 6.f), line, 1.f);
            dl->AddLine(ImVec2(c.x + 3.f, c.y + 2.f), ImVec2(c.x - 5.f, c.y - 6.f), line, 1.f);
        }
        else if (glyph == 4)
        {
            dl->AddRectFilled(ImVec2(c.x - 1.f, c.y - 6.f), ImVec2(c.x + 5.f, c.y + 3.f), white);
            dl->AddRect(ImVec2(c.x - 1.f, c.y - 6.f), ImVec2(c.x + 5.f, c.y + 3.f), line);
            dl->AddRectFilled(ImVec2(c.x - 5.f, c.y - 2.f), ImVec2(c.x + 1.f, c.y + 7.f), white);
            dl->AddRect(ImVec2(c.x - 5.f, c.y - 2.f), ImVec2(c.x + 1.f, c.y + 7.f), line);
        }
        else
        {
            const ImU32 tan = on ? IM_COL32(205, 175, 120, 255) : style::Shadow;
            const ImVec2 a(c.x - 5.f, c.y - 5.f);
            const ImVec2 b(c.x + 5.f, c.y + 7.f);
            dl->AddRectFilled(a, b, tan);
            dl->AddRect(a, b, line);
            dl->AddRectFilled(ImVec2(c.x - 2.f, a.y - 2.f), ImVec2(c.x + 2.f, a.y + 1.f), style::Shadow);
            dl->AddRectFilled(ImVec2(a.x + 2.f, a.y + 3.f), ImVec2(b.x - 2.f, b.y - 1.f), white);
        }
    }

    void BeginToolbar()
    {
        s_tbX = ksd::BodyLeft() + 2.f;
        s_tbY = ksd::ToolbarTop() + 2.f;

        ImDrawList* dl = ImGui::GetWindowDrawList();
        const float y = ksd::ToolbarTop() + style::ToolbarH - 1.f;
        dl->AddLine(ImVec2(ksd::BodyLeft(), y), ImVec2(ksd::BodyRight(), y), style::Shadow, 1.f);
        dl->AddLine(ImVec2(ksd::BodyLeft(), y + 1.f), ImVec2(ksd::BodyRight(), y + 1.f), style::Highlight, 1.f);
    }

    void EndToolbar()
    {
    }

    bool ToolButton(const char* id, int glyph, const char* tooltip, bool enabled)
    {
        (void)id;
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const float sz = style::ToolbarH - 4.f;
        const ImVec2 a(s_tbX, s_tbY);
        const ImVec2 b(a.x + sz, a.y + sz);

        const bool hovered = enabled && ImGui::IsMouseHoveringRect(a, b) && ksd::PopUpsAreClosed();
        const bool held = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

        if (held)
        {
            style::FillFace(dl, a, b);
            style::Pressed(dl, a, b);
        }
        else if (hovered)
        {
            style::FillFace(dl, a, b);
            style::Raised(dl, a, b);
        }

        const float push = held ? 1.f : 0.f;
        Glyph(dl, ImVec2((a.x + b.x) * 0.5f + push, (a.y + b.y) * 0.5f + push), glyph, enabled);

        if (hovered && tooltip != nullptr)
        {
            ImDrawList* fg = ImGui::GetForegroundDrawList();
            const ImVec2 m = ImGui::GetMousePos();
            const ImVec2 ts = ImGui::CalcTextSize(tooltip);
            const ImVec2 ta(m.x + 12.f, m.y + 18.f);
            const ImVec2 tb(ta.x + ts.x + 8.f, ta.y + ts.y + 4.f);
            fg->AddRectFilled(ta, tb, IM_COL32(255, 255, 225, 255));
            fg->AddRect(ta, tb, style::DarkShadow);
            fg->AddText(ImVec2(ta.x + 4.f, ta.y + 2.f), style::Text, tooltip);
        }

        s_tbX += sz + 1.f;
        return enabled && hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    }

    void ToolSeparator()
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const float sz = style::ToolbarH - 4.f;
        const float x = s_tbX + 3.f;
        dl->AddLine(ImVec2(x, s_tbY + 1.f), ImVec2(x, s_tbY + sz - 1.f), style::Shadow, 1.f);
        dl->AddLine(ImVec2(x + 1.f, s_tbY + 1.f), ImVec2(x + 1.f, s_tbY + sz - 1.f), style::Highlight, 1.f);
        s_tbX += 8.f;
    }
}
