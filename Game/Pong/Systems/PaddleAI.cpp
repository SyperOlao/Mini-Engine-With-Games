//
// Created by SyperOlao on 18.03.2026.
//
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "PaddleAI.h"
#include <algorithm>
#include <cmath>

#include "Core/Common/Constants.h"
#include "Core/Math/MathHelpers.h"
#include "Game/Pong/Entities/Ball.h"
#include "Game/Pong/Entities/Paddle.h"


void PaddleAI::Reset() noexcept
{
    m_state = {};
}

void PaddleAI::Update(Paddle& paddle, const Ball& ball, const DifficultyTuning& tuning, const float deltaTime)
{
    if (m_state.MistakeTimer > 0.0f)
    {
        m_state.MistakeTimer = std::max(0.0f, m_state.MistakeTimer - deltaTime);
        if (m_state.MistakeTimer == 0.0f)
        {
            m_state.CurrentTrackingOffset = 0.0f;
        }
    }
    else if (MathHelpers::RandomFloat(0.0f, 1.0f) < tuning.AIMistakeChancePerSecond * deltaTime)
    {
        m_state.MistakeTimer = MathHelpers::RandomFloat(tuning.AIMistakeDurationMin, tuning.AIMistakeDurationMax);
        m_state.CurrentTrackingOffset = MathHelpers::RandomFloat(-tuning.AITrackingError, tuning.AITrackingError);
    }

    const float ballCenterY = ball.GetAABB().GetCenter().y;
    const float paddleCenterY = paddle.GetAABB().GetCenter().y;
    const float targetCenterY = ballCenterY + m_state.CurrentTrackingOffset;
    const float deltaY = targetCenterY - paddleCenterY;

    if (std::abs(deltaY) <= Constants::PaddleAIDeadZone)
    {
        return;
    }

    float moveSpeed = tuning.AIPaddleSpeed;
    if (m_state.MistakeTimer > 0.0f)
    {
        moveSpeed *= tuning.AIMistakeSpeedMultiplier;
    }

    const float direction = deltaY > 0.0f ? 1.0f : -1.0f;
    paddle.Transform.Position.y += direction * moveSpeed * deltaTime;
}