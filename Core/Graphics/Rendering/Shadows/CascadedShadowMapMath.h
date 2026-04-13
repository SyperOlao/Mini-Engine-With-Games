#ifndef PINGPONG_CASCADEDSHADOWMAPMATH_H
#define PINGPONG_CASCADEDSHADOWMAPMATH_H

#include <SimpleMath.h>

#include <array>
#include <cstdint>

struct CascadedShadowSplitCpu final
{
    float NearPlaneDistance{0.1f};
    float FarPlaneDistance{100.0f};
    std::array<float, 5> ViewSpaceSplitBoundaries{};
};

[[nodiscard]] CascadedShadowSplitCpu ComputePracticalCascadeSplits(
    float cameraNearPlane,
    float cameraFarPlane,
    std::uint32_t cascadeCount,
    float lambdaBlend
);

[[nodiscard]] DirectX::SimpleMath::Matrix BuildDirectionalCascadeLightViewProjection(
    const DirectX::SimpleMath::Vector3 &lightDirectionWorld,
    const DirectX::SimpleMath::Matrix &cameraViewMatrix,
    float viewSpaceNearZ,
    float viewSpaceFarZ,
    float verticalFieldOfViewRadians,
    float aspectRatio,
    std::uint32_t cascadeResolutionPixels,
    float lightSpaceDepthPadding
);

#endif
