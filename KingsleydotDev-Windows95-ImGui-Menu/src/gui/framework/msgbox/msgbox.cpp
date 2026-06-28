#include "msgbox.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

#include <cstring>

namespace ksd
{
    static constexpr int kMaxLinks = 4;

    static bool s_open = false;
    static char s_id[64] = { 0 };
    static char s_title[128] = { 0 };
    static char s_text[256] = { 0 };

    static char s_linkLabel[kMaxLinks][64] = {};
    static char s_linkUrl[kMaxLinks][256] = {};
    static int s_linkCount = 0;

    static char s_resultId[64] = { 0 };
    static int s_result = 0;

    static void CopyStr(char* dst, const char* src, size_t cap)
    {
        size_t i = 0;
        for (; src[i] != '\0' && i + 1 < cap; ++i)
        {
            dst[i] = src[i];
        }
        dst[i] = '\0';
    }

    static bool DialogButton(ImDrawList* fg, const ImVec2& a, const ImVec2& b, const char* label)
    {
        const bool hovered = ImGui::IsMouseHoveringRect(a, b, false);
        const bool held = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
        style::FillFace(fg, a, b);
        if (held)
        {
            style::Pressed(fg, a, b);
        }
        else
        {
            style::Raised(fg, a, b);
        }
        const float push = held ? 1.f : 0.f;
        const ImVec2 ts = ImGui::CalcTextSize(label);
        fg->AddText(ImVec2((a.x + b.x - ts.x) * 0.5f + push, (a.y + b.y - ts.y) * 0.5f + push), style::Text, label);
        return hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    }

    void OpenMessageBox(const char* id, const char* title, const char* text)
    {
        OpenMessageBox(id, title, text, nullptr, 0);
    }

    void OpenMessageBox(const char* id, const char* title, const char* text, const MessageLink* links, int linkCount)
    {
        CopyStr(s_id, id, sizeof(s_id));
        CopyStr(s_title, title, sizeof(s_title));
        CopyStr(s_text, text, sizeof(s_text));

        s_linkCount = linkCount < kMaxLinks ? linkCount : kMaxLinks;
        for (int i = 0; i < s_linkCount; ++i)
        {
            CopyStr(s_linkLabel[i], links[i].label, sizeof(s_linkLabel[i]));
            CopyStr(s_linkUrl[i], links[i].url, sizeof(s_linkUrl[i]));
        }

        s_open = true;
        ksd::SetSelectedItem("##msgbox");
    }

    void DrawMessageBox()
    {
        s_result = 0;
        s_resultId[0] = '\0';
        if (!s_open)
        {
            return;
        }

        ImDrawList* fg = ImGui::GetForegroundDrawList();
        const float lineH = ImGui::GetTextLineHeight();
        const float titleH = 16.f;
        const float btnW = 64.f;
        const float btnH = 22.f;
        const float linkStep = lineH + 5.f;

        const float textTop = 3.f + titleH + 16.f;
        const float linksTop = textTop + lineH + 8.f;
        const float btnTop = linksTop + s_linkCount * linkStep + 12.f;

        const float bw = 250.f;
        const float bh = btnTop + btnH + 10.f;
        const float cx = (ksd::BodyLeft() + ksd::BodyRight()) * 0.5f;
        const float cy = (ksd::BodyTop() + ksd::BodyBottom()) * 0.5f;
        const ImVec2 a(cx - bw * 0.5f, cy - bh * 0.5f);
        const ImVec2 b(cx + bw * 0.5f, cy + bh * 0.5f);

        style::FillFace(fg, a, b);
        style::Raised(fg, a, b);

        const ImVec2 ta(a.x + 3.f, a.y + 3.f);
        const ImVec2 tb(b.x - 3.f, a.y + 3.f + titleH);
        fg->AddRectFilled(ta, tb, ksd::AccentColor());
        fg->AddText(ImVec2(ta.x + 4.f, ta.y + (titleH - lineH) * 0.5f), style::TitleText, s_title);

        fg->AddText(ImVec2(a.x + 16.f, a.y + textTop), style::Text, s_text);

        const ImU32 linkCol = IM_COL32(0, 0, 200, 255);
        for (int i = 0; i < s_linkCount; ++i)
        {
            const ImVec2 lp(a.x + 16.f, a.y + linksTop + i * linkStep);
            const ImVec2 ls = ImGui::CalcTextSize(s_linkLabel[i]);
            const ImVec2 lb(lp.x + ls.x, lp.y + ls.y);
            fg->AddText(lp, linkCol, s_linkLabel[i]);
            fg->AddLine(ImVec2(lp.x, lb.y - 1.f), ImVec2(lb.x, lb.y - 1.f), linkCol, 1.f);
            if (ImGui::IsMouseHoveringRect(lp, lb, false) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                ksd::OpenUrl(s_linkUrl[i]);
            }
        }

        const float by = a.y + btnTop;
        const ImVec2 okA(b.x - btnW * 2.f - 18.f, by);
        const ImVec2 okB(okA.x + btnW, by + btnH);
        const ImVec2 caA(b.x - btnW - 10.f, by);
        const ImVec2 caB(caA.x + btnW, by + btnH);

        int result = 0;
        if (DialogButton(fg, okA, okB, "OK"))
        {
            result = 1;
        }
        if (DialogButton(fg, caA, caB, "Cancel"))
        {
            result = 2;
        }

        if (result != 0)
        {
            s_result = result;
            CopyStr(s_resultId, s_id, sizeof(s_resultId));
            s_open = false;
            ksd::SetSelectedItem(nullptr);
        }
    }

    int MessageBoxResult(const char* id)
    {
        if (s_result != 0 && std::strcmp(s_resultId, id) == 0)
        {
            return s_result;
        }
        return 0;
    }
}
