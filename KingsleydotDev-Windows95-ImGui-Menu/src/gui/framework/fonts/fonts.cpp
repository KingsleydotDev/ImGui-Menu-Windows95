#include "fonts.hpp"
#include "imgui.h"

extern "C" unsigned char micross[876080];

namespace fonts
{
    static constexpr float kFontSize = 16.f;

    void Init()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImFontConfig cfg;
        cfg.FontDataOwnedByAtlas = false;
        cfg.OversampleH = 2;
        cfg.OversampleV = 2;
        cfg.PixelSnapH = true;

        io.Fonts->AddFontFromMemoryTTF(micross, (int)sizeof(micross), kFontSize, &cfg);
    }
}
