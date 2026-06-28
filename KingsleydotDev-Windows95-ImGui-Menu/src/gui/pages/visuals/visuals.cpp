#include "visuals.hpp"
#include "../pages.hpp"
#include "../../framework/framework.hpp"

namespace vars
{
    bool  visFullbright = false;
    bool  visNightMode = false;
    bool  visNoFog = true;
    bool  visNoSky = false;
    bool  visWireframe = false;
    float visAmbient = 0.50f;
    float visSaturation = 1.f;
    float visBrightness = 1.f;
    int   visRenderMode = 0;

    bool  visChams = true;
    int   visChamsStyle = 0;
    bool  visChamsVisible = true;
    float visChamsVisibleColor[4] = { 0.20f, 0.55f, 0.90f, 1.f };
    bool  visChamsHidden = true;
    float visChamsHiddenColor[4] = { 0.90f, 0.30f, 0.30f, 1.f };
    bool  visChamsHiddenRainbow = false;
    float visChamsHiddenSpeed = 1.f;
    int   visChamsMaterial = 0;

    bool  visGlow = false;
    bool  visGlowColor = true;
    float visGlowColorRgba[4] = { 0.30f, 0.80f, 1.f, 1.f };
    float visGlowIntensity = 1.4f;
    int   visGlowStyle = 0;

    bool  visBloom = false;
    bool  visDof = false;
    bool  visVignette = false;
    bool  visMotionBlur = false;
    int   visEffectFov = 90;
}

namespace pages
{
    void RenderVisuals() noexcept
    {
        static const char* const renderItems[] = { "normal", "flat", "matte", "chrome" };
        static const char* const materialItems[] = { "plastic", "metal", "glass", "neon" };
        static const char* const glowStyleItems[] = { "outline", "full", "pulse" };

        ksd::SetColumns(2);

        if (ksd::BeginGroup("world", 0))
        {
            ksd::Checkbox("fullbright", &vars::visFullbright);
            ksd::Checkbox("night mode", &vars::visNightMode);
            ksd::Checkbox("no fog", &vars::visNoFog);
            ksd::Checkbox("no sky", &vars::visNoSky);
            ksd::Checkbox("wireframe", &vars::visWireframe);

            ksd::SeparatorText("colour grading");
            ksd::SliderFloat("ambient", &vars::visAmbient, 0.f, 1.f, "%.2f");
            ksd::SliderFloat("saturation", &vars::visSaturation, 0.f, 2.f);
            ksd::SliderFloat("brightness", &vars::visBrightness, 0.f, 2.f);
            ksd::Combo("render mode", &vars::visRenderMode, renderItems, 4);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("chams", 0))
        {
            ksd::Checkbox("enable", &vars::visChams);
            ksd::Separator();
            ksd::Label("style");
            ksd::RadioButton("flat", &vars::visChamsStyle, 0);
            ksd::RadioButton("textured", &vars::visChamsStyle, 1);
            ksd::RadioButton("glow", &vars::visChamsStyle, 2);
            ksd::Separator();
            ksd::CheckboxColor("visible", &vars::visChamsVisible, "vis_chams_vis", vars::visChamsVisibleColor);
            ksd::CheckboxColor("hidden", &vars::visChamsHidden, "vis_chams_hid", vars::visChamsHiddenColor, &vars::visChamsHiddenRainbow, &vars::visChamsHiddenSpeed);
            ksd::Combo("material", &vars::visChamsMaterial, materialItems, 4);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("glow", 1))
        {
            ksd::Checkbox("enable", &vars::visGlow);
            ksd::CheckboxColor("colour", &vars::visGlowColor, "vis_glow_col", vars::visGlowColorRgba);
            ksd::SliderFloat("intensity", &vars::visGlowIntensity, 0.f, 5.f);
            ksd::Combo("style", &vars::visGlowStyle, glowStyleItems, 3);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("effects", 1))
        {
            ksd::Checkbox("bloom", &vars::visBloom);
            ksd::Checkbox("depth of field", &vars::visDof);
            ksd::Checkbox("vignette", &vars::visVignette);
            ksd::Checkbox("motion blur", &vars::visMotionBlur);
            ksd::SliderInt("fov", &vars::visEffectFov, 70, 130);
        }
        ksd::EndGroup();
    }
}
