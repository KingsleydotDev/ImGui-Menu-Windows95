#include "aimbot.hpp"
#include "../pages.hpp"
#include "../../framework/framework.hpp"

namespace vars
{
    bool  aimEnabled = false;
    int   kbAimX = ImGuiKey_F;
    bool  aimSilent = false;
    bool  aimNoRecoil = true;
    bool  aimVisCheck = true;
    int   aimFov = 90;
    float aimSmoothing = 5.f;
    float aimRcsAmount = 65.f;
    int   aimTargetBone = 0;
    int   aimTargetMode = 3;
    int   aimKeyType = 0;

    bool  aimHitboxes[7] = { true, false, true, false, false, false, false };
    bool  aimAutoScale = true;
    int   aimMultipoint = 4;

    bool  espEnabled = true;
    bool  espBoxes = true;
    float espBoxColor[4] = { 0.86f, 0.20f, 0.20f, 1.f };
    bool  espNames = true;
    float espNameColor[4] = { 1.f, 1.f, 1.f, 1.f };
    bool  espHealth = true;
    float espHealthColor[4] = { 0.10f, 0.80f, 0.16f, 1.f };
    bool  espHealthRainbow = false;
    float espHealthSpeed = 1.f;
    bool  espArmor = false;
    float espArmorColor[4] = { 0.30f, 0.55f, 0.95f, 1.f };
    bool  espDistance = true;
    bool  espSkeleton = false;
    float espSkeletonColor[4] = { 1.f, 1.f, 1.f, 1.f };
    bool  espSnaplines = false;
    float espSnapColor[4] = { 0.95f, 0.90f, 0.30f, 1.f };
    int   espMaxDist = 392;
    int   espBoxStyle = 0;

    bool  aimPrediction = true;
    bool  aimFovCircle = true;
    bool  aimFovCircleShow = true;
    float aimFovCircleColor[4] = { 0.95f, 0.95f, 0.95f, 0.40f };
    int   aimPingSpike = 0;
}

namespace pages
{
    void RenderAimbot() noexcept
    {
        static const char* const boneItems[] = { "head", "neck", "chest", "stomach", "pelvis" };
        static const char* const targetItems[] = { "closest", "low hp", "distance", "crosshair" };
        static const char* const keyTypeItems[] = { "hold", "toggle", "always" };
        static const char* const hitboxItems[] = { "head", "neck", "chest", "stomach", "pelvis", "arms", "legs" };
        static const char* const boxStyleItems[] = { "2d", "corner", "filled", "3d" };

        ksd::SetColumns(2);

        if (ksd::BeginGroup("aimbot", 0))
        {
            ksd::CheckboxKeybind("enable", &vars::aimEnabled, "aim_enable_kb", &vars::kbAimX);
            ksd::Checkbox("silent aim", &vars::aimSilent);
            ksd::Checkbox("no recoil", &vars::aimNoRecoil);
            ksd::Checkbox("visibility check", &vars::aimVisCheck);
            ksd::Tooltip("only target players in line of sight");

            ksd::SeparatorText("tuning");
            ksd::SliderInt("fov", &vars::aimFov, 0, 180);
            ksd::SliderFloat("smoothing", &vars::aimSmoothing, 0.f, 20.f);
            ksd::SliderFloat("rcs amount", &vars::aimRcsAmount, 0.f, 100.f, "%.0f%%");
            ksd::Combo("target bone", &vars::aimTargetBone, boneItems, 5);
            ksd::Combo("target mode", &vars::aimTargetMode, targetItems, 4);
            ksd::Combo("aim key type", &vars::aimKeyType, keyTypeItems, 3);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("hitboxes", 0))
        {
            ksd::MultiCombo("enabled hitboxes", vars::aimHitboxes, hitboxItems, 7);
            ksd::Checkbox("auto scale", &vars::aimAutoScale);
            ksd::Tooltip("shrink hitbox at long range");
            ksd::SliderInt("multipoint", &vars::aimMultipoint, 1, 8);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("esp", 1))
        {
            ksd::Checkbox("enable", &vars::espEnabled);
            ksd::CheckboxColor("boxes", &vars::espBoxes, "esp_boxes", vars::espBoxColor);
            ksd::CheckboxColor("name", &vars::espNames, "esp_name", vars::espNameColor);
            ksd::CheckboxColor("health", &vars::espHealth, "esp_health", vars::espHealthColor, &vars::espHealthRainbow, &vars::espHealthSpeed);
            ksd::CheckboxColor("armor", &vars::espArmor, "esp_armor", vars::espArmorColor);
            ksd::Checkbox("distance", &vars::espDistance);
            ksd::CheckboxColor("skeleton", &vars::espSkeleton, "esp_skel", vars::espSkeletonColor);
            ksd::CheckboxColor("snaplines", &vars::espSnaplines, "esp_snap", vars::espSnapColor);
            ksd::SliderInt("max distance", &vars::espMaxDist, 0, 1000);
            ksd::Combo("box style", &vars::espBoxStyle, boxStyleItems, 4);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("advanced", 1))
        {
            if (ksd::TreeNode("advanced"))
            {
                ksd::Checkbox("prediction", &vars::aimPrediction);
                ksd::Tooltip("compensate for target velocity");
                ksd::Checkbox("fov circle", &vars::aimFovCircle);
                ksd::CheckboxColor("fov circle colour", &vars::aimFovCircleShow, "aim_fov_col", vars::aimFovCircleColor);
                ksd::SliderInt("ping spike", &vars::aimPingSpike, 0, 350);
                ksd::TreePop();
            }
        }
        ksd::EndGroup();
    }
}
