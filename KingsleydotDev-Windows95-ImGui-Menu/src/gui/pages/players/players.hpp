#pragma once

namespace vars
{
    extern int   plrSelected;
    extern bool  plrFriend;
    extern bool  plrIgnoreAim;
    extern bool  plrForceVisible;
    extern bool  plrWhitelist;
    extern bool  plrTags[5];

    extern bool  plrDrawChams;
    extern float plrChamsColor[4];
    extern bool  plrGlow;
    extern float plrGlowColor[4];
    extern bool  plrGlowRainbow;
    extern float plrGlowSpeed;
    extern bool  plrName;
    extern float plrNameColor[4];
    extern float plrPriority;
    extern int   plrHitbox;

    extern int   plrAimPriority;
    extern int   plrMaxDistance;
    extern int   plrBacktrackTicks;
    extern bool  plrAutoMark;
    extern bool  plrShowOffscreen;
    extern int   plrSortMode;

    extern bool  plrLogJoins;
    extern bool  plrLogLeaves;
    extern bool  plrLogChat;
    extern bool  plrAnnounceFriends;
    extern int   plrLogScrollback;
    extern char  plrNoteBuf[64];

    extern bool  plrPanicHide;
    extern int   plrCacheSize;
    extern float plrSessionShare;
}
