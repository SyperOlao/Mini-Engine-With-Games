//
// Created by SyperOlao on 17.03.2026.
//

#ifndef PINGPONG_CONSTANTS_H
#define PINGPONG_CONSTANTS_H
#include <Windows.h>
#include "Types.h"

namespace Constants
{
    inline constexpr int WindowWidth = 1280;
    inline constexpr int WindowHeight = 720;

    inline constexpr float PaddleWidth = 20.0f;
    inline constexpr float PaddleHeight = 120.0f;
    inline constexpr float BallSize = 12.0f;

    inline constexpr float PaddleMarginX = 40.0f;

    inline constexpr float WallThickness = 16.0f;

    inline constexpr float CenterLineWidth = 6.0f;
    inline constexpr float CenterLineSegmentHeight = 18.0f;
    inline constexpr float CenterLineGap = 12.0f;

    inline constexpr float PaddleSpeed = 720.0f;
    inline constexpr float PaddleAIMaxSpeed = 680.0f;
    inline constexpr float PaddleAIDeadZone = 10.0f;

    inline constexpr float BallStartSpeed = 420.0f;
    inline constexpr float BallMaxSpeed = 1500.0f;
    inline constexpr float BallSpeedMultiplierOnPaddleHit = 1.05f;
    inline constexpr float BallResetDelaySeconds = 0.75f;

    inline constexpr float MaxBounceAngleDegrees = 60.0f;

    inline constexpr ScoreType ScoreToWin = 10;

    namespace Input
    {
        inline constexpr USHORT Player1Up = 'W';
        inline constexpr USHORT Player1Down = 'S';
        inline constexpr USHORT Player2Up = VK_UP;
        inline constexpr USHORT Player2Down = VK_DOWN;

        inline constexpr USHORT Pause = VK_ESCAPE;
        inline constexpr USHORT Restart = VK_RETURN;
        inline constexpr USHORT ToggleMode = VK_F1;
    }
}

#endif //PINGPONG_CONSTANTS_H