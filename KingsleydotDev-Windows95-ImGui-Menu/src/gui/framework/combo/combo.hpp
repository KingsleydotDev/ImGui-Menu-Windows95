#pragma once

#include "imgui.h"

namespace ksd
{
    bool Combo(const char* label, int* current, const char* const items[], int count);

    bool MultiCombo(const char* label, bool* selected, const char* const items[], int count);
}
