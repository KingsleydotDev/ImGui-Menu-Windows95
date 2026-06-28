#pragma once

#include "imgui.h"

namespace images
{
    void Init(void* d3dDevice, void* d3dContext);

    ImTextureID Icon();
}
