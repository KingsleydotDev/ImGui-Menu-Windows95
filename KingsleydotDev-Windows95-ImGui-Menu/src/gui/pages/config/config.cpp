#include "config.hpp"
#include "../pages.hpp"
#include "../../framework/framework.hpp"

namespace vars
{
    bool  xhairEnable = false;
    float xhairColor[4] = { 1.f, 1.f, 1.f, 1.f };
    float xhairOutline[4] = { 0.f, 0.f, 0.f, 1.f };
    float xhairSize = 4.f;
    float xhairGap = 2.f;
    float xhairThickness = 1.f;
    float xhairOutlineThickness = 1.f;
    int   xhairStyle = 0;
    bool  xhairCenterDot = true;
    bool  xhairShowEnemy = false;

    bool  ovWatermark = true;
    bool  ovShowFps = true;
    bool  ovShowPing = false;
    bool  ovKeybindList = false;
    bool  ovHitmarker = false;
    float ovHmColor[4] = { 1.f, 1.f, 1.f, 1.f };
    int   ovHmSound = 0;
    float ovHmSize = 5.f;

    bool  cfgAccentOn = true;
    float cfgAccent[4] = { 0.f, 0.f, 0.5f, 1.f };
    bool  cfgAccentRainbow = false;
    float cfgAccentSpeed = 1.f;
    int   cfgTheme = 0;
    float cfgMenuScale = 1.f;
    float cfgBackgroundAlpha = 0.9f;

    char  cfgProfile[32] = "default";
    int   cfgQuality = 1;
    int   cfgThreads = 4;
    bool  cfgVerboseLogging = false;
    bool  cfgSafeMode = true;
    bool  cfgAutoSave = true;
}

namespace pages
{
    void RenderConfig() noexcept
    {
        static const char* const xhairStyles[] = { "cross", "t-style", "circle", "dot" };
        static const char* const hmSounds[] = { "click", "bell", "thud", "none" };
        static const char* const themes[] = { "classic navy", "teal", "plum", "slate", "hotdog" };

        ksd::SetColumns(2);

        if (ksd::BeginGroup("crosshair", 0))
        {
            const ImVec2 row = ksd::CursorPos();
            ksd::Checkbox("enable", &vars::xhairEnable);
            ksd::ColorButton("xhair_fill", vars::xhairColor,
                ImVec2(ksd::PanelRightX() - style::SwatchW * 2.f - 2.f, row.y), ImVec2(style::SwatchW, style::SwatchH));
            ksd::ColorButton("xhair_outline", vars::xhairOutline,
                ImVec2(ksd::PanelRightX() - style::SwatchW, row.y), ImVec2(style::SwatchW, style::SwatchH));

            ksd::SliderFloat("size", &vars::xhairSize, 0.f, 10.f);
            ksd::SliderFloat("gap", &vars::xhairGap, 0.f, 10.f);
            ksd::SliderFloat("thickness", &vars::xhairThickness, 0.f, 10.f);
            ksd::SliderFloat("outline thickness", &vars::xhairOutlineThickness, 0.f, 10.f);

            ksd::Combo("style", &vars::xhairStyle, xhairStyles, 4);
            ksd::Checkbox("center dot", &vars::xhairCenterDot);
            ksd::Checkbox("show on enemy", &vars::xhairShowEnemy);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("overlay", 1))
        {
            ksd::Checkbox("watermark", &vars::ovWatermark);
            ksd::Checkbox("show fps", &vars::ovShowFps);
            ksd::Checkbox("show ping", &vars::ovShowPing);
            ksd::Checkbox("keybind list", &vars::ovKeybindList);

            const ImVec2 row = ksd::CursorPos();
            ksd::Checkbox("hitmarker", &vars::ovHitmarker);
            ksd::ColorButton("ov_hm", vars::ovHmColor,
                ImVec2(ksd::PanelRightX() - style::SwatchW, row.y), ImVec2(style::SwatchW, style::SwatchH));

            ksd::Combo("hitmarker sound", &vars::ovHmSound, hmSounds, 4);
            ksd::SliderFloat("hitmarker size", &vars::ovHmSize, 0.f, 10.f);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("appearance", 1))
        {
            ksd::CheckboxColor("menu accent", &vars::cfgAccentOn, "cfg_accent", vars::cfgAccent,
                &vars::cfgAccentRainbow, &vars::cfgAccentSpeed);

            ksd::Label("theme");
            ksd::Combo("theme", &vars::cfgTheme, themes, 5);
            ksd::SliderFloat("menu scale", &vars::cfgMenuScale, 0.5f, 2.0f);
            ksd::SliderFloat("background alpha", &vars::cfgBackgroundAlpha, 0.f, 1.f);
        }
        ksd::EndGroup();

        ksd::SetAccent(vars::cfgAccent, vars::cfgAccentOn);

        if (ksd::BeginGroup("config file", 0))
        {
            ksd::SeparatorText("profile");
            ksd::InputText("cfg_name", vars::cfgProfile, (int)sizeof(vars::cfgProfile));

            ksd::SeparatorText("quality");
            ksd::RadioButton("low", &vars::cfgQuality, 0);
            ksd::RadioButton("medium", &vars::cfgQuality, 1);
            ksd::RadioButton("high", &vars::cfgQuality, 2);

            ksd::SpinInt("threads", &vars::cfgThreads, 1, 16);

            if (ksd::TreeNode("advanced"))
            {
                ksd::Checkbox("verbose logging", &vars::cfgVerboseLogging);
                ksd::Checkbox("safe mode", &vars::cfgSafeMode);
                ksd::Checkbox("auto-save", &vars::cfgAutoSave);
                ksd::TreePop();
            }

            ksd::Label("download progress");
            ksd::ProgressBar(0.65f);

            const float w = ksd::ContentWidth();
            ksd::Button("save config", w);
            ksd::Button("load config", w);
            if (ksd::Button("reset defaults", w))
            {
                ksd::OpenMessageBox("cfg_reset", "Confirm", "Reset all settings to defaults?");
            }

            ksd::Hyperlink("visit project page");
        }
        ksd::EndGroup();

        if (ksd::MessageBoxResult("cfg_reset") == 1)
        {
            vars::xhairSize = 4.f;
            vars::xhairGap = 2.f;
        }
    }
}
