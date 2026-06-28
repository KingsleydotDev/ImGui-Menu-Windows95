#pragma once

#include "imgui.h"

namespace ksd
{
    bool Checkbox(const char* label, bool* v);

    bool CheckboxKeybind(const char* label, bool* on, const char* id, int* key);

    bool CheckboxColor(const char* label, bool* on, const char* id, float col[4],
        bool* rainbow = nullptr, float* speed = nullptr);
}
