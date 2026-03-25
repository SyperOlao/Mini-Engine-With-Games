#include "KatamariCameraController.h"

#include "Core/Graphics/FollowCamera.h"

using DirectX::SimpleMath::Vector3;

void KatamariCameraController::Initialize(FollowCamera &followCamera) noexcept
{
    m_followCamera = &followCamera;
}

void KatamariCameraController::Update(
    const float deltaTime,
    const Vector3 &ballWorldPosition,
    const Vector3 &ballVelocityWorld,
    const float ballRadiusWorld
)
{
    if (m_followCamera == nullptr)
    {
        return;
    }

    Vector3 velocityHorizontal = ballVelocityWorld;
    velocityHorizontal.y = 0.0f;
    if (velocityHorizontal.LengthSquared() > 1.0e-6f)
    {
        velocityHorizontal.Normalize();
        m_followCamera->SetMovementDirectionXZ(velocityHorizontal);
    }

    m_followCamera->SetTarget(ballWorldPosition);
    m_followCamera->SetBallRadiusForZoom(ballRadiusWorld);
    m_followCamera->Update(deltaTime);
}
