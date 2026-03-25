#include "Core/Graphics/FollowCamera.h"

#include <algorithm>
#include <cmath>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

void FollowCamera::SetTarget(const Vector3 &target) noexcept
{
    m_target = target;
}

void FollowCamera::SetOffsetFromTarget(const Vector3 &offsetFromTarget) noexcept
{
    m_offsetFromTarget = offsetFromTarget;
}

void FollowCamera::SetMovementDirectionXZ(const Vector3 &directionWorld) noexcept
{
    m_movementDirectionXZ = directionWorld;
}

void FollowCamera::SetPositionLagSeconds(const float seconds) noexcept
{
    m_positionLagSeconds = (std::max)(seconds, 1.0e-4f);
}

void FollowCamera::SetOrientationLagSeconds(const float seconds) noexcept
{
    m_orientationLagSeconds = (std::max)(seconds, 1.0e-4f);
}

void FollowCamera::SetBallRadiusForZoom(const float radiusWorld) noexcept
{
    m_ballRadius = (std::max)(radiusWorld, 1.0e-4f);
}

void FollowCamera::SetZoomFromRadiusParameters(
    const float referenceRadius,
    const float minimumZoomScale,
    const float maximumZoomScale
) noexcept
{
    m_referenceBallRadius = (std::max)(referenceRadius, 1.0e-4f);
    m_minimumZoomScale = (std::max)(minimumZoomScale, 1.0e-4f);
    m_maximumZoomScale = (std::max)(maximumZoomScale, m_minimumZoomScale);
}

void FollowCamera::Update(const float deltaTime) noexcept
{
    Vector3 forwardHorizontal = m_movementDirectionXZ;
    forwardHorizontal.y = 0.0f;
    if (forwardHorizontal.LengthSquared() < 1.0e-8f)
    {
        forwardHorizontal = m_smoothedForwardHorizontal;
    }
    else
    {
        forwardHorizontal.Normalize();
    }

    if (!m_hasSmoothedForward)
    {
        m_smoothedForwardHorizontal = forwardHorizontal;
        m_hasSmoothedForward = true;
    }
    else
    {
        const float orientationAlpha = 1.0f - std::exp(-deltaTime / m_orientationLagSeconds);
        m_smoothedForwardHorizontal = Vector3::Lerp(m_smoothedForwardHorizontal, forwardHorizontal, orientationAlpha);
        if (m_smoothedForwardHorizontal.LengthSquared() > 1.0e-8f)
        {
            m_smoothedForwardHorizontal.Normalize();
        }
        else
        {
            m_smoothedForwardHorizontal = Vector3(0.0f, 0.0f, 1.0f);
        }
    }

    const Vector3 desiredEye = ComputeDesiredEyePosition();
    if (!m_hasSmoothedEye)
    {
        m_smoothedEyePosition = desiredEye;
        m_hasSmoothedEye = true;
        return;
    }

    const float positionAlpha = 1.0f - std::exp(-deltaTime / m_positionLagSeconds);
    m_smoothedEyePosition = Vector3::Lerp(m_smoothedEyePosition, desiredEye, positionAlpha);
}

Vector3 FollowCamera::ComputeDesiredEyePosition() const noexcept
{
    Vector3 forwardHorizontal = m_smoothedForwardHorizontal;
    forwardHorizontal.y = 0.0f;
    if (forwardHorizontal.LengthSquared() < 1.0e-8f)
    {
        forwardHorizontal = Vector3(0.0f, 0.0f, 1.0f);
    }
    else
    {
        forwardHorizontal.Normalize();
    }

    const float yawRadians = std::atan2(forwardHorizontal.x, forwardHorizontal.z);
    const Matrix yawRotation = Matrix::CreateRotationY(yawRadians);

    const float radiusRatio = m_referenceBallRadius / m_ballRadius;
    const float zoomScale = (std::clamp)(radiusRatio, m_minimumZoomScale, m_maximumZoomScale);
    const Vector3 scaledOffset = m_offsetFromTarget * zoomScale;
    const Vector3 rotatedOffset = Vector3::TransformNormal(scaledOffset, yawRotation);
    return m_target + rotatedOffset;
}

Vector3 FollowCamera::GetPosition() const noexcept
{
    if (!m_hasSmoothedEye)
    {
        return ComputeDesiredEyePosition();
    }

    return m_smoothedEyePosition;
}

Matrix FollowCamera::GetViewMatrix() const
{
    const Vector3 eyePosition = GetPosition();
    return Matrix::CreateLookAt(eyePosition, m_target, Vector3::Up);
}
