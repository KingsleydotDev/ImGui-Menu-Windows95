#pragma once

#include "imgui.h"

namespace ksd
{
    bool ColorButton(const char* id, float col[4], const ImVec2& pos, const ImVec2& size,
        bool* rainbow = nullptr, float* speed = nullptr);
}
