#include "skins.hpp"
#include "../pages.hpp"
#include "../../framework/framework.hpp"

namespace vars
{
    int   sknWeapon = 0;
    int   sknSkin = 1;
    float sknWear = 0.06f;
    int   sknSeed = 387;
    bool  sknStattrak = true;
    int   sknStattrakKills = 1337;
    char  sknNameTag[64] = "knife";

    int   sknKnifeModel = 0;
    int   sknGloveModel = 0;
    bool  sknCustomPaint = false;
    bool  sknPaintEnabled = true;
    float sknPaintColor[4] = { 0.85f, 0.22f, 0.30f, 1.f };
    bool  sknPaintRainbow = false;
    float sknPaintSpeed = 1.f;
}

namespace pages
{
    void RenderSkins() noexcept
    {
        static const char* const weapons[] = { "ak-47", "m4a4", "awp", "usp-s", "glock", "deagle", "famas", "mp9" };
        static const char* const skins[] = { "default", "dragon lore", "asiimov", "fade", "marble fade", "hyper beast" };
        static const char* const knives[] = { "karambit", "butterfly", "m9", "talon", "skeleton" };
        static const char* const gloves[] = { "sport", "specialist", "driver", "hand wraps" };

        ksd::SetColumns(2);

        if (ksd::BeginGroup("weapon", 0))
        {
            ksd::ListBox("skn_weapon", &vars::sknWeapon, weapons, 8);
            ksd::Tooltip("pick a weapon to edit");
            ksd::Separator();
            ksd::Combo("skin", &vars::sknSkin, skins, 6);
            ksd::SliderFloat("wear", &vars::sknWear, 0.f, 1.f, "%.2f");
            ksd::SpinInt("seed", &vars::sknSeed, 0, 1000);
            ksd::Checkbox("stattrak", &vars::sknStattrak);
            ksd::SpinInt("stattrak kills", &vars::sknStattrakKills, 0, 9999);
            ksd::Label("name tag");
            ksd::InputText("skn_name", vars::sknNameTag, (int)sizeof(vars::sknNameTag));
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("knife & gloves", 1))
        {
            ksd::Combo("knife model", &vars::sknKnifeModel, knives, 5);
            ksd::Combo("glove model", &vars::sknGloveModel, gloves, 4);
            ksd::Separator();
            ksd::Checkbox("custom paint", &vars::sknCustomPaint);
            ksd::CheckboxColor("paint colour", &vars::sknPaintEnabled, "skn_paint", vars::sknPaintColor, &vars::sknPaintRainbow, &vars::sknPaintSpeed);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("actions", 1))
        {
            const float buttonWidth = ksd::ContentWidth();
            ksd::Button("apply", buttonWidth);
            ksd::Button("reset", buttonWidth);
            ksd::Button("randomize", buttonWidth);
        }
        ksd::EndGroup();
    }
}
