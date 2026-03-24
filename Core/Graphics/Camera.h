//
// Created by SyperOlao on 19.03.2026.
//

#ifndef PINGPONG_CAMERA_H
#define PINGPONG_CAMERA_H
#include <SimpleMath.h>

#include "ProjectionMode.h"


class Camera {
public:
    virtual ~Camera() = default;

    [[nodiscard]] virtual DirectX::SimpleMath::Matrix GetViewMatrix() const = 0;

    [[nodiscard]] DirectX::SimpleMath::Matrix GetProjectionMatrix(const float aspectRatio) const {
        using namespace DirectX::SimpleMath;

        switch (m_projectionMode)
        {
            case ProjectionMode::PerspectiveOffCenter:
                return Matrix::CreatePerspectiveOffCenter(
                    -aspectRatio, aspectRatio,
                    -1.0f, 1.0f,
                    0.1f, 2000.0f
                );

            case ProjectionMode::PerspectiveFov:
            default:
                return Matrix::CreatePerspectiveFieldOfView(
                    DirectX::XMConvertToRadians(60.0f),
                    aspectRatio,
                    0.1f,
                    2000.0f
                );
        }
    }

    void SetProjectionMode(const ProjectionMode mode) noexcept
    {
        m_projectionMode = mode;
    }

    [[nodiscard]] ProjectionMode GetProjectionMode() const noexcept
    {
        return m_projectionMode;
    }

protected:
    ProjectionMode m_projectionMode{ProjectionMode::PerspectiveFov};
};


#endif //PINGPONG_CAMERA_H