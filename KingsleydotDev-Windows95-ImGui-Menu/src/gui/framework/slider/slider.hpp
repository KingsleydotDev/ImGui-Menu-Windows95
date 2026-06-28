#pragma once

#include "imgui.h"

namespace ksd
{
    bool SliderInt(const char* label, int* v, int vmin, int vmax);
    bool SliderFloat(const char* label, float* v, float vmin, float vmax, const char* fmt = "%.1f");

    void DrawTrackbar(ImDrawList* dl, const ImVec2& origin, float w, float t, bool focused);
}
