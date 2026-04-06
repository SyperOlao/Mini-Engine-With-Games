#ifndef PINGPONG_KATAMARISPHERECOLLIDERWORLD_H
#define PINGPONG_KATAMARISPHERECOLLIDERWORLD_H

#include "Core/Gameplay/GameplayComponents.h"

#include <SimpleMath.h>

#include <algorithm>

namespace KatamariSphereColliderWorld
{
[[nodiscard]] inline float ComputeMaximumWorldScaleAxis(DirectX::SimpleMath::Matrix const &worldMatrix) noexcept
{
    using DirectX::SimpleMath::Vector3;
    Vector3 const basisX(worldMatrix._11, worldMatrix._12, worldMatrix._13);
    Vector3 const basisY(worldMatrix._21, worldMatrix._22, worldMatrix._23);
    Vector3 const basisZ(worldMatrix._31, worldMatrix._32, worldMatrix._33);
    return (std::max)(basisX.Length(), (std::max)(basisY.Length(), basisZ.Length()));
}

inline void ComputeWorldCenterAndRadius(
    TransformComponent const &transform,
    SphereColliderComponent const &sphere,
    DirectX::SimpleMath::Vector3 &outWorldCenter,
    float &outWorldRadius
) noexcept
{
    using DirectX::SimpleMath::Vector3;
    outWorldCenter = Vector3::Transform(sphere.LocalCenter, transform.WorldMatrix);
    outWorldRadius = sphere.Radius * ComputeMaximumWorldScaleAxis(transform.WorldMatrix);
}
}

#endif
