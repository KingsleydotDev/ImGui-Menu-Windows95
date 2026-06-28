#include "treenode.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

#include <cstring>

namespace ksd
{
    static void CopyStr(char* dst, const char* src, size_t cap)
    {
        size_t i = 0;
        for (; src[i] != '\0' && i + 1 < cap; ++i)
        {
            dst[i] = src[i];
        }
        dst[i] = '\0';
    }

    static bool& OpenFor(const char* label)
    {
        static char ids[16][64] = {};
        static bool open[16] = {};
        for (int i = 0; i < 16; ++i)
        {
            if (ids[i][0] == '\0')
            {
                CopyStr(ids[i], label, sizeof(ids[i]));
                open[i] = false;
                return open[i];
            }
            if (std::strcmp(ids[i], label) == 0)
            {
                return open[i];
            }
        }
        return open[0];
    }

    bool TreeNode(const char* label)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        bool& open = OpenFor(label);

        const float box = 9.f;
        const ImVec2 ba(pos.x, pos.y + (style::RowHeight - box) * 0.5f);
        const ImVec2 bb(ba.x + box, ba.y + box);
        dl->AddRectFilled(ba, bb, style::Field);
        dl->AddRect(ba, bb, style::Shadow);
        const ImVec2 c((ba.x + bb.x) * 0.5f, (ba.y + bb.y) * 0.5f);
        dl->AddLine(ImVec2(c.x - 2.f, c.y), ImVec2(c.x + 3.f, c.y), style::Text, 1.f);
        if (!open)
        {
            dl->AddLine(ImVec2(c.x, c.y - 2.f), ImVec2(c.x, c.y + 3.f), style::Text, 1.f);
        }

        const ImVec2 ts = ImGui::CalcTextSize(label);
        const float tx = bb.x + 5.f;
        dl->AddText(ImVec2(tx, pos.y + (style::RowHeight - ts.y) * 0.5f), style::Text, label);

        const ImVec2 hitMax(tx + ts.x + 2.f, pos.y + style::RowHeight);
        if (ImGui::IsMouseHoveringRect(pos, hitMax) && ksd::PopUpsAreClosed() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            open = !open;
        }

        ksd::AddItem(ImVec2(ksd::ContentWidth(), style::RowHeight));
        if (open)
        {
            ksd::Indent(16.f);
        }
        return open;
    }

    void TreePop()
    {
        ksd::Unindent(16.f);
    }
}
