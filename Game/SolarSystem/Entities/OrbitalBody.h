//
// Created by SyperOlao on 19.03.2026.
//

#ifndef PINGPONG_ORBITALBODY_H
#define PINGPONG_ORBITALBODY_H

#include <SimpleMath.h>
#include <memory>
#include <vector>
#include "Core/Math/OrbitMath.h"

enum class BodyMeshType : std::uint8_t
{
    Sphere = 0,
    Box
};

class OrbitalBody final
{
public:
    OrbitalBody() = default;

    void Update(float deltaTime);
    void UpdateWorldMatrix(const DirectX::SimpleMath::Matrix& parentWorld);

    void AddChild(std::shared_ptr<OrbitalBody> child);

    [[nodiscard]] const DirectX::SimpleMath::Matrix& GetWorldMatrix() const noexcept;
    [[nodiscard]] const std::vector<std::shared_ptr<OrbitalBody>>& GetChildren() const noexcept;

public:
    BodyMeshType MeshType{BodyMeshType::Sphere};

    DirectX::SimpleMath::Vector3 Scale{1.0f, 1.0f, 1.0f};

    OrbitalParams Orbit{};
    bool HasOrbit{false};

    float SelfRotationAngle{0.0f};
    float SelfRotationSpeed{0.0f};

private:
    float m_currentMeanAnomaly{0.0f};
    DirectX::SimpleMath::Matrix m_world{DirectX::SimpleMath::Matrix::Identity};

    std::vector<std::shared_ptr<OrbitalBody>> m_children;
};


#endif //PINGPONG_ORBITALBODY_H