//
// Created by SyperOlao on 18.03.2026.
//
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "PongCollisionSystem.h"

#include <algorithm>
#include <cmath>

#include "Core/Common/Constants.h"
#include "Core/Math/MathHelpers.h"
#include "Core/Physics/CollisionSystem.h"
#include "Game/Pong/Entities/Ball.h"
#include "Game/Pong/Entities/Paddle.h"

namespace PongCollisionSystem
{
    void HandleWallCollision(Ball& ball, const float playableTop, const float playableBottom) noexcept
    {
        const AABB ballAABB = ball.GetAABB();

        if (ballAABB.Min.y <= playableTop)
        {
            ball.Transform.Position.y = playableTop;
            ball.Movement.Velocity.y = std::abs(ball.Movement.Velocity.y);
        }
        else if (ballAABB.Max.y >= playableBottom)
        {
            ball.Transform.Position.y = playableBottom - ball.Size();
            ball.Movement.Velocity.y = -std::abs(ball.Movement.Velocity.y);
        }
    }

    void HandlePaddleCollision(Ball& ball, const Paddle& leftPaddle, const Paddle& rightPaddle)
    {
        const AABB ballAABB = ball.GetAABB();
        const AABB leftPaddleAABB = leftPaddle.GetAABB();
        const AABB rightPaddleAABB = rightPaddle.GetAABB();

        if (CollisionSystem::CheckCollision(ballAABB, leftPaddleAABB) && ball.Movement.Velocity.x < 0.0f)
        {
            ball.Transform.Position.x = leftPaddleAABB.Max.x + 0.5f;

            const float nextSpeed = std::min(
                ball.Movement.Velocity.Length() * Constants::BallSpeedMultiplierOnPaddleHit,
                Constants::BallMaxSpeed
            );

            ball.Movement.Velocity = CollisionSystem::BuildPaddleBounceVelocity(
                ball.GetAABB(),
                leftPaddleAABB,
                nextSpeed,
                MathHelpers::DegToRad(Constants::MaxBounceAngleDegrees),
                true
            );

            return;
        }

        if (CollisionSystem::CheckCollision(ballAABB, rightPaddleAABB) && ball.Movement.Velocity.x > 0.0f)
        {
            ball.Transform.Position.x = rightPaddleAABB.Min.x - ball.Size() - 0.5f;

            const float nextSpeed = std::min(
                ball.Movement.Velocity.Length() * Constants::BallSpeedMultiplierOnPaddleHit,
                Constants::BallMaxSpeed
            );

            ball.Movement.Velocity = CollisionSystem::BuildPaddleBounceVelocity(
                ball.GetAABB(),
                rightPaddleAABB,
                nextSpeed,
                MathHelpers::DegToRad(Constants::MaxBounceAngleDegrees),
                false
            );
        }
    }

    CourtSide CheckScoring(const Ball& ball) noexcept
    {
        return CollisionSystem::CheckOutOfBounds(
            ball.GetAABB(),
            0.0f,
            static_cast<float>(Constants::WindowWidth)
        );
    }
}