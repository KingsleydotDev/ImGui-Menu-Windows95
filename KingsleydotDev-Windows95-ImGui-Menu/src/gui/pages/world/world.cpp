#include "world.hpp"
#include "../pages.hpp"
#include "../../framework/framework.hpp"

namespace vars
{
    bool  wldCustomSky = false;
    int   kbWldSky = ImGuiKey_None;
    bool  wldSkyTint = true;
    float wldSkyColor[4] = { 0.36f, 0.58f, 0.92f, 1.f };
    int   wldTimeOfDay = 1;
    float wldSunSize = 1.4f;
    bool  wldStars = false;
    float wldCloudDensity = 0.5f;

    bool  wldCustomFog = false;
    bool  wldFogTint = true;
    float wldFogColor[4] = { 0.62f, 0.66f, 0.70f, 1.f };
    bool  wldFogRainbow = false;
    float wldFogSpeed = 1.f;
    float wldFogDensity = 0.35f;
    int   wldGravity = 800;
    bool  wldWeather[5] = { true, false, false, false, false };
    bool  wldRemoveProps = false;
    bool  wldRemoveWater = false;

    float wldAmbient = 1.f;
    bool  wldShadows = true;
    bool  wldLightTint = false;
    float wldLightColor[4] = { 1.f, 0.95f, 0.85f, 1.f };
    int   wldLightMode = 0;

    bool  wldThirdPerson = false;
    int   wldFov = 90;
    float wldCameraDistance = 2.f;
    float wldCameraSmooth = 0.5f;
}

namespace pages
{
    void RenderWorld() noexcept
    {
        static const char* const timeItems[] = { "dawn", "noon", "dusk", "midnight" };
        static const char* const weatherItems[] = { "rain", "snow", "wind", "storm", "fog" };
        static const char* const lightModes[] = { "realistic", "flat", "vivid" };

        ksd::SetColumns(2);

        if (ksd::BeginGroup("sky", 0))
        {
            ksd::CheckboxKeybind("custom sky", &vars::wldCustomSky, "wld_sky_kb", &vars::kbWldSky);
            ksd::CheckboxColor("sky colour", &vars::wldSkyTint, "wld_sky_col", vars::wldSkyColor);
            ksd::Combo("time of day", &vars::wldTimeOfDay, timeItems, 4);
            ksd::SliderFloat("sun size", &vars::wldSunSize, 0.f, 5.f);
            ksd::SeparatorText("atmosphere");
            ksd::Checkbox("stars", &vars::wldStars);
            ksd::SliderFloat("cloud density", &vars::wldCloudDensity, 0.f, 1.f, "%.2f");
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("environment", 1))
        {
            ksd::Checkbox("custom fog", &vars::wldCustomFog);
            ksd::CheckboxColor("fog colour", &vars::wldFogTint, "wld_fog_col", vars::wldFogColor, &vars::wldFogRainbow, &vars::wldFogSpeed);
            ksd::SliderFloat("fog density", &vars::wldFogDensity, 0.f, 1.f, "%.2f");
            ksd::SpinInt("gravity", &vars::wldGravity, 0, 2000);
            ksd::MultiCombo("weather", vars::wldWeather, weatherItems, 5);
            ksd::Separator();
            ksd::Checkbox("remove props", &vars::wldRemoveProps);
            ksd::Checkbox("remove water", &vars::wldRemoveWater);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("lighting", 0))
        {
            ksd::SliderFloat("ambient", &vars::wldAmbient, 0.f, 2.f, "%.2f");
            ksd::Checkbox("shadows", &vars::wldShadows);
            ksd::CheckboxColor("light colour", &vars::wldLightTint, "wld_light_col", vars::wldLightColor);
            ksd::Combo("mode", &vars::wldLightMode, lightModes, 3);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("camera", 1))
        {
            ksd::Checkbox("thirdperson", &vars::wldThirdPerson);
            ksd::SliderInt("fov", &vars::wldFov, 70, 130);
            ksd::SliderFloat("camera distance", &vars::wldCameraDistance, 0.f, 10.f, "%.2f");
            ksd::SliderFloat("smooth", &vars::wldCameraSmooth, 0.f, 1.f, "%.2f");
        }
        ksd::EndGroup();
    }
}
