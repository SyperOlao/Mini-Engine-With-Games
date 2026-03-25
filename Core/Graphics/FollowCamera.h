#ifndef PINGPONG_FOLLOWCAMERA_H
#define PINGPONG_FOLLOWCAMERA_H

#include "Core/Graphics/Camera.h"

class FollowCamera final : public Camera
{
public:
    void SetTarget(const DirectX::SimpleMath::Vector3 &target) noexcept;

    void SetOffsetFromTarget(const DirectX::SimpleMath::Vector3 &offsetFromTarget) noexcept;

    void SetMovementDirectionXZ(const DirectX::SimpleMath::Vector3 &directionWorld) noexcept;

    void SetPositionLagSeconds(float seconds) noexcept;

    void SetOrientationLagSeconds(float seconds) noexcept;

    void SetBallRadiusForZoom(float radiusWorld) noexcept;

    void SetZoomFromRadiusParameters(float referenceRadius, float minimumZoomScale, float maximumZoomScale) noexcept;

    void Update(float deltaTime) noexcept;

    [[nodiscard]] DirectX::SimpleMath::Vector3 GetPosition() const noexcept override;

    [[nodiscard]] DirectX::SimpleMath::Matrix GetViewMatrix() const override;

private:
    [[nodiscard]] DirectX::SimpleMath::Vector3 ComputeDesiredEyePosition() const noexcept;

private:
    DirectX::SimpleMath::Vector3 m_target{0.0f, 0.0f, 0.0f};
    DirectX::SimpleMath::Vector3 m_offsetFromTarget{0.0f, 8.0f, -20.0f};
    DirectX::SimpleMath::Vector3 m_movementDirectionXZ{0.0f, 0.0f, 1.0f};
    float m_positionLagSeconds{0.12f};
    float m_orientationLagSeconds{0.18f};
    float m_ballRadius{1.0f};
    float m_referenceBallRadius{1.0f};
    float m_minimumZoomScale{0.75f};
    float m_maximumZoomScale{2.5f};
    DirectX::SimpleMath::Vector3 m_smoothedEyePosition{0.0f, 0.0f, 0.0f};
    bool m_hasSmoothedEye{false};
    DirectX::SimpleMath::Vector3 m_smoothedForwardHorizontal{0.0f, 0.0f, 1.0f};
    bool m_hasSmoothedForward{false};
};

#endif
