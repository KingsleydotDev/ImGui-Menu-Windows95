#include "players.hpp"
#include "../pages.hpp"
#include "../../framework/framework.hpp"

namespace vars
{
    int   plrSelected = 0;
    bool  plrFriend = false;
    bool  plrIgnoreAim = false;
    bool  plrForceVisible = false;
    bool  plrWhitelist = false;
    bool  plrTags[5] = { false, false, true, false, false };

    bool  plrDrawChams = true;
    float plrChamsColor[4] = { 0.78f, 0.30f, 0.86f, 1.f };
    bool  plrGlow = false;
    float plrGlowColor[4] = { 0.20f, 0.55f, 0.90f, 1.f };
    bool  plrGlowRainbow = false;
    float plrGlowSpeed = 1.f;
    bool  plrName = true;
    float plrNameColor[4] = { 1.f, 1.f, 1.f, 1.f };
    float plrPriority = 5.f;
    int   plrHitbox = 0;

    int   plrAimPriority = 1;
    int   plrMaxDistance = 600;
    int   plrBacktrackTicks = 12;
    bool  plrAutoMark = false;
    bool  plrShowOffscreen = true;
    int   plrSortMode = 0;

    bool  plrLogJoins = true;
    bool  plrLogLeaves = false;
    bool  plrLogChat = false;
    bool  plrAnnounceFriends = true;
    int   plrLogScrollback = 200;
    char  plrNoteBuf[64] = "rage on sight";

    bool  plrPanicHide = false;
    int   plrCacheSize = 32;
    float plrSessionShare = 0.42f;
}

namespace pages
{
    void RenderPlayers() noexcept
    {
        static const char* const playerNames[] = { "n0thing", "shroud", "s1mple", "Kingsley",
            "ScreaM", "device", "NiKo", "olofm" };
        static const char* const playerTags[] = { "cheater", "friend", "legit", "rage", "suspect" };
        static const char* const hitboxes[] = { "head", "chest", "pelvis", "nearest" };
        static const char* const aimPriorities[] = { "lowest", "normal", "high", "highest" };
        static const char* const sortModes[] = { "name", "distance", "priority", "tag" };

        ksd::SetColumns(2);

        if (ksd::BeginGroup("players", 0))
        {
            ksd::ListBox("plr_list", &vars::plrSelected, playerNames, 8, 5);
            ksd::Tooltip("select a player");
            ksd::Separator();
            ksd::Checkbox("friend", &vars::plrFriend);
            ksd::Checkbox("ignore aimbot", &vars::plrIgnoreAim);
            ksd::Checkbox("force visible", &vars::plrForceVisible);
            ksd::Checkbox("whitelist", &vars::plrWhitelist);
            ksd::MultiCombo("tags", vars::plrTags, playerTags, 5);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("targeting", 0))
        {
            ksd::Combo("aim priority", &vars::plrAimPriority, aimPriorities, 4);
            ksd::SliderInt("max distance", &vars::plrMaxDistance, 0, 2000);
            ksd::SliderInt("backtrack ticks", &vars::plrBacktrackTicks, 0, 16);
            ksd::Checkbox("auto mark new players", &vars::plrAutoMark);
            ksd::Checkbox("show offscreen", &vars::plrShowOffscreen);
            ksd::Combo("list sort", &vars::plrSortMode, sortModes, 4);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("overrides", 1))
        {
            ksd::Checkbox("draw chams", &vars::plrDrawChams);
            ksd::CheckboxColor("chams colour", &vars::plrDrawChams, "plr_chams_col", vars::plrChamsColor);
            ksd::CheckboxColor("glow colour", &vars::plrGlow, "plr_glow_col", vars::plrGlowColor, &vars::plrGlowRainbow, &vars::plrGlowSpeed);
            ksd::CheckboxColor("name colour", &vars::plrName, "plr_name_col", vars::plrNameColor);
            ksd::Separator();
            ksd::SliderFloat("priority", &vars::plrPriority, 0.f, 10.f);
            ksd::Combo("aim hitbox", &vars::plrHitbox, hitboxes, 4);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("event log", 1))
        {
            ksd::Checkbox("log joins", &vars::plrLogJoins);
            ksd::Checkbox("log leaves", &vars::plrLogLeaves);
            ksd::Checkbox("log chat", &vars::plrLogChat);
            ksd::Checkbox("announce friends", &vars::plrAnnounceFriends);
            ksd::SpinInt("scrollback", &vars::plrLogScrollback, 50, 1000, 50);
            ksd::InputText("plr_note", vars::plrNoteBuf, 64);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("storage", 1))
        {
            ksd::SeparatorText("session");
            ksd::Label("cached players");
            ksd::ProgressBar(vars::plrSessionShare);
            ksd::SpinInt("cache size", &vars::plrCacheSize, 8, 128, 8);
            ksd::Checkbox("hide list on panic", &vars::plrPanicHide);
        }
        ksd::EndGroup();

        if (ksd::BeginGroup("actions", 1))
        {
            ksd::Button("mark friend");
            ksd::Button("mark enemy");
            ksd::Button("clear list", ksd::ContentWidth());
        }
        ksd::EndGroup();
    }
}
