#include "keybind.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

#include <cstring>
#include <cctype>

namespace ksd
{
    static char s_capturingId[64] = { 0 };

    static void CopyStr(char* dst, const char* src, size_t cap)
    {
        size_t i = 0;
        for (; src[i] != '\0' && i + 1 < cap; ++i)
        {
            dst[i] = src[i];
        }
        dst[i] = '\0';
    }

    static void KeyLabel(int key, char* out, int outSize)
    {
        if (key == ImGuiKey_None)
        {
            CopyStr(out, "-", (size_t)outSize);
            return;
        }
        const char* name = ImGui::GetKeyName((ImGuiKey)key);
        int i = 0;
        for (; name[i] != '\0' && i < outSize - 1; ++i)
        {
            out[i] = (char)std::tolower((unsigned char)name[i]);
        }
        out[i] = '\0';
    }

    bool Keybind(const char* id, int* key, const ImVec2& pos, const ImVec2& size)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 a = pos;
        const ImVec2 b(pos.x + size.x, pos.y + size.y);

        dl->AddRectFilled(a, b, style::Field);
        style::Sunken(dl, a, b);

        const bool capturing = std::strcmp(s_capturingId, id) == 0;
        const bool hovering = ImGui::IsMouseHoveringRect(a, b);
        bool triggered = false;

        if (capturing)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) ||
                (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !hovering))
            {
                s_capturingId[0] = '\0';
                ksd::SetSelectedItem(nullptr);
            }
            else
            {
                for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; ++k)
                {
                    const ImGuiKey kk = (ImGuiKey)k;
                    if (kk == ImGuiKey_Escape || kk == ImGuiKey_MouseLeft || kk == ImGuiKey_MouseRight)
                    {
                        continue;
                    }
                    if (ImGui::IsKeyPressed(kk, false))
                    {
                        *key = k;
                        s_capturingId[0] = '\0';
                        ksd::SetSelectedItem(nullptr);
                        break;
                    }
                }
            }
        }
        else if (hovering && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ksd::PopUpsAreClosed())
        {
            CopyStr(s_capturingId, id, sizeof(s_capturingId));
            ksd::SetSelectedItem(id);
        }
        else if (hovering && ksd::PopUpsAreClosed() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            *key = ImGuiKey_None;
        }
        else if (*key != ImGuiKey_None && ksd::PopUpsAreClosed() &&
            ImGui::IsKeyPressed((ImGuiKey)*key, false))
        {
            triggered = true;
        }

        char label[24];
        if (capturing)
        {
            CopyStr(label, "...", sizeof(label));
        }
        else
        {
            KeyLabel(*key, label, sizeof(label));
        }

        const ImVec2 ts = ImGui::CalcTextSize(label);
        dl->AddText(ImVec2((a.x + b.x - ts.x) * 0.5f, (a.y + b.y - ts.y) * 0.5f), style::Text, label);

        return triggered;
    }
}
