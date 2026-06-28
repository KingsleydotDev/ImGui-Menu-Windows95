#pragma once

namespace vars
{
    extern bool  aimEnabled;
    extern int   kbAimX;
    extern bool  aimSilent;
    extern bool  aimNoRecoil;
    extern bool  aimVisCheck;
    extern int   aimFov;
    extern float aimSmoothing;
    extern float aimRcsAmount;
    extern int   aimTargetBone;
    extern int   aimTargetMode;
    extern int   aimKeyType;

    extern bool  aimHitboxes[7];
    extern bool  aimAutoScale;
    extern int   aimMultipoint;

    extern bool  espEnabled;
    extern bool  espBoxes;
    extern float espBoxColor[4];
    extern bool  espNames;
    extern float espNameColor[4];
    extern bool  espHealth;
    extern float espHealthColor[4];
    extern bool  espHealthRainbow;
    extern float espHealthSpeed;
    extern bool  espArmor;
    extern float espArmorColor[4];
    extern bool  espDistance;
    extern bool  espSkeleton;
    extern float espSkeletonColor[4];
    extern bool  espSnaplines;
    extern float espSnapColor[4];
    extern int   espMaxDist;
    extern int   espBoxStyle;

    extern bool  aimPrediction;
    extern bool  aimFovCircle;
    extern bool  aimFovCircleShow;
    extern float aimFovCircleColor[4];
    extern int   aimPingSpike;
}
