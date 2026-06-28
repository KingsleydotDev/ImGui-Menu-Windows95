#include "misc.hpp"
#include "../pages.hpp"
#include "../../framework/framework.hpp"

namespace vars
{
    bool  miscBhop = false;
    int   mscKeyBhop = ImGuiKey_Space;
    bool  miscAutoStrafe = false;
    int   mscKeyAutoStrafe = ImGuiKey_None;
    bool  miscEdgeJump = false;
    int   mscKeyEdgeJump = ImGuiKey_None;
    bool  miscFastStop = true;
    bool  miscAutoJump = false;
    float miscAirSpeed = 1.f;
    float miscAirStrafe = 1.f;
    int   miscJumpMode = 0;

    bool  miscFly = false;
    int   mscKeyFly = ImGuiKey_F;
    bool  miscNoclip = false;
    int   mscKeyNoclip = ImGuiKey_N;
    bool  miscAntiAim = false;
    int   miscFakeLag = 0;
    float miscDesync = 0.f;
    bool  miscJitter = false;
    int   miscJitterRange = 30;
    int   miscRoll = 0;

    float miscAnimSpeed = 4.f;
    bool  miscWatermark = true;
    bool  miscShowFps = true;
    bool  miscBlur = true;
    int   miscTheme = 0;
    int   mscPanicKey = ImGuiKey_Delete;
    float miscSessionXp = 0.42f;
}

namespace pages
{
    void RenderMisc() noexcept
    {
        static const char* const jumpModes[] = { "on ground", "in air", "always" };
        static const char* const rollModes[] = { "off", "left", "right", "random" };
        static const char* const themes[] = { "navy", "teal", "plum", "slate" };

        ksd::SetColumns(2);

        if (ksd::BeginGroup("movement", 0))
        {
            ksd::CheckboxKeybind("bunny hop", &vars::miscBhop, "msc_bhop", &vars::mscKeyBhop);
            ksd::CheckboxKeybind("auto strafe", &vars::miscAutoStrafe, "msc_strafe", &vars::mscKeyAutoStrafe);
            ksd::CheckboxKeybind("edge jump", &vars::miscEdgeJump, "msc_edge", &vars::mscKeyEdgeJump);
            ksd::Checkbox("fast stop", &vars::miscFastStop);
            ksd::Checkbox("auto jump", &vars::miscAutoJump);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("strafe tuning", 0))
        {
            ksd::SliderFloat("air speed", &vars::miscAirSpeed, 0.f, 5.f, "%.2f");
            ksd::SliderFloat("air strafe", &vars::miscAirStrafe, 0.f, 5.f, "%.2f");
            ksd::Combo("jump mode", &vars::miscJumpMode, jumpModes, 3);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("exploits", 0))
        {
            ksd::CheckboxKeybind("fly", &vars::miscFly, "msc_fly", &vars::mscKeyFly);
            ksd::CheckboxKeybind("noclip", &vars::miscNoclip, "msc_noclip", &vars::mscKeyNoclip);
            ksd::Checkbox("anti-aim", &vars::miscAntiAim);
            ksd::SliderInt("fake lag", &vars::miscFakeLag, 0, 16);
            ksd::SliderFloat("desync", &vars::miscDesync, 0.f, 60.f);

            if (ksd::TreeNode("advanced"))
            {
                ksd::Checkbox("jitter", &vars::miscJitter);
                ksd::SliderInt("jitter range", &vars::miscJitterRange, 0, 180);
                ksd::Combo("roll", &vars::miscRoll, rollModes, 4);
                ksd::TreePop();
            }
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("menu", 1))
        {
            ksd::SliderFloat("anim speed", &vars::miscAnimSpeed, 0.f, 10.f);
            ksd::Checkbox("watermark", &vars::miscWatermark);
            ksd::Checkbox("show fps", &vars::miscShowFps);
            ksd::Checkbox("blur", &vars::miscBlur);
            ksd::Combo("theme", &vars::miscTheme, themes, 4);

            ksd::Separator();

            const ImVec2 row = ksd::CursorPos();
            ksd::Label("panic key");
            ksd::Keybind("msc_panic", &vars::mscPanicKey, ImVec2(ksd::PanelRightX() - style::KeybindW, row.y), ImVec2(style::KeybindW, style::KeybindH));
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("session", 1))
        {
            ksd::SeparatorText("progress");
            ksd::Label("rank progress");
            ksd::ProgressBar(vars::miscSessionXp);

            ksd::Separator();

            ksd::Button("save settings", ksd::ContentWidth());
            if (ksd::Button("unload", ksd::ContentWidth()))
            {
                vars::miscAnimSpeed = 4.f;
            }

            ksd::Separator();

            ksd::Label("Made by KingsleydotDev");
            if (ksd::Hyperlink("YouTube"))
            {
                ksd::OpenUrl("https://www.youtube.com/watch?v=MMVKjO2yi94");
            }
            if (ksd::Hyperlink("Discord"))
            {
                ksd::OpenUrl("https://discord.gg/7JfVtn2cwE");
            }
        }
        ksd::EndGroup();
    }
}
