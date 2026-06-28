#include "slider.hpp"
#include "../shell/shell.hpp"
#include "../styling.hpp"
#include "imgui.h"

#include <cstdio>

namespace ksd
{
    static const void* s_active = nullptr;

    static void DottedRect(ImDrawList* dl, const ImVec2& a, const ImVec2& b, ImU32 col)
    {
        for (float x = a.x; x < b.x; x += 2.f)
        {
            dl->AddRectFilled(ImVec2(x, a.y), ImVec2(x + 1.f, a.y + 1.f), col);
            dl->AddRectFilled(ImVec2(x, b.y - 1.f), ImVec2(x + 1.f, b.y), col);
        }
        for (float y = a.y; y < b.y; y += 2.f)
        {
            dl->AddRectFilled(ImVec2(a.x, y), ImVec2(a.x + 1.f, y + 1.f), col);
            dl->AddRectFilled(ImVec2(b.x - 1.f, y), ImVec2(b.x, y + 1.f), col);
        }
    }

    void DrawTrackbar(ImDrawList* dl, const ImVec2& origin, float w, float t, bool focused)
    {
        const float left = origin.x;
        const float right = origin.x + w;
        const float trackY = origin.y;

        if (focused)
        {
            DottedRect(dl, ImVec2(left - 2.f, trackY - 2.f),
                ImVec2(right + 2.f, trackY + style::SliderTrackH), style::Text);
        }

        const float grooveY = trackY + 6.f;
        dl->AddLine(ImVec2(left, grooveY), ImVec2(right, grooveY), style::Shadow, 1.f);
        dl->AddLine(ImVec2(left, grooveY + 1.f), ImVec2(right, grooveY + 1.f), style::Highlight, 1.f);

        const float tickTop = trackY + style::ThumbBodyH + style::ThumbPointH + 1.f;
        int tickCount = (int)(w / 16.f);
        if (tickCount < 4) { tickCount = 4; }
        for (int i = 0; i <= tickCount; ++i)
        {
            const float tx = left + (w - 1.f) * (float)i / (float)tickCount;
            dl->AddLine(ImVec2(tx, tickTop), ImVec2(tx, tickTop + style::TickH), style::Shadow, 1.f);
        }

        const float cx = left + style::ThumbW * 0.5f + t * (w - style::ThumbW);
        const float halfW = style::ThumbW * 0.5f;
        const float bodyB = trackY + style::ThumbBodyH;
        const ImVec2 tl(cx - halfW, trackY);
        const ImVec2 tr(cx + halfW, trackY);
        const ImVec2 br(cx + halfW, bodyB);
        const ImVec2 pt(cx, bodyB + style::ThumbPointH);
        const ImVec2 bl(cx - halfW, bodyB);
        const ImVec2 poly[5] = { tl, tr, br, pt, bl };
        dl->AddConvexPolyFilled(poly, 5, style::Face);

        dl->AddRectFilled(ImVec2(tl.x, tl.y), ImVec2(tr.x, tl.y + 1.f), style::Highlight);
        dl->AddRectFilled(ImVec2(tl.x, tl.y), ImVec2(tl.x + 1.f, bl.y), style::Highlight);
        dl->AddRectFilled(ImVec2(tl.x + 1.f, tl.y + 1.f), ImVec2(tr.x - 1.f, tl.y + 2.f), style::Light);
        dl->AddRectFilled(ImVec2(tl.x + 1.f, tl.y + 1.f), ImVec2(tl.x + 2.f, bl.y), style::Light);
        dl->AddRectFilled(ImVec2(tr.x - 1.f, tr.y), ImVec2(tr.x, br.y), style::DarkShadow);
        dl->AddRectFilled(ImVec2(tr.x - 2.f, tr.y + 1.f), ImVec2(tr.x - 1.f, br.y), style::Shadow);
        dl->AddLine(ImVec2(bl.x + 0.5f, bl.y), pt, style::Highlight, 1.f);
        dl->AddLine(ImVec2(br.x - 0.5f, br.y), pt, style::DarkShadow, 1.f);
    }

    static float DrawSlider(const char* label, float value, float vmin, float vmax, const char* fmt, const void* id)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 pos = ksd::CursorPos();
        const float w = ksd::ContentWidth();
        const float lineH = ImGui::GetTextLineHeight();

        char buf[32];
        std::snprintf(buf, sizeof(buf), fmt, value);
        const ImVec2 vts = ImGui::CalcTextSize(buf);

        dl->AddText(pos, style::Text, label);
        dl->AddText(ImVec2(pos.x + w - vts.x, pos.y), style::Text, buf);

        const float trackY = pos.y + lineH + 3.f;
        const float left = pos.x;
        const float right = pos.x + w;
        const ImVec2 trackMin(left, trackY);
        const ImVec2 trackMax(right, trackY + style::SliderTrackH);

        const float span = (vmax - vmin);
        float t = (span != 0.f) ? (value - vmin) / span : 0.f;
        if (t < 0.f) { t = 0.f; }
        if (t > 1.f) { t = 1.f; }

        const bool hovered = ImGui::IsMouseHoveringRect(trackMin, trackMax) && ksd::PopUpsAreClosed();
        if (s_active == id)
        {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                s_active = nullptr;
            }
        }
        else if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            s_active = id;
        }

        if (s_active == id)
        {
            const float mx = ImGui::GetMousePos().x;
            float nt = (mx - (left + style::ThumbW * 0.5f)) / (w - style::ThumbW);
            if (nt < 0.f) { nt = 0.f; }
            if (nt > 1.f) { nt = 1.f; }
            t = nt;
            value = vmin + t * span;
        }

        DrawTrackbar(dl, ImVec2(left, trackY), w, t, hovered || s_active == id);

        ksd::AddItem(ImVec2(w, lineH + 3.f + style::SliderTrackH));
        return value;
    }

    bool SliderInt(const char* label, int* v, int vmin, int vmax)
    {
        const float nv = DrawSlider(label, (float)*v, (float)vmin, (float)vmax, "%.0f", v);
        const int iv = (int)(nv + (nv >= 0.f ? 0.5f : -0.5f));
        const bool changed = iv != *v;
        *v = iv;
        return changed;
    }

    bool SliderFloat(const char* label, float* v, float vmin, float vmax, const char* fmt)
    {
        const float nv = DrawSlider(label, *v, vmin, vmax, fmt, v);
        const bool changed = nv != *v;
        *v = nv;
        return changed;
    }
}
