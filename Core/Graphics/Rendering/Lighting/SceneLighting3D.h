#ifndef PINGPONG_SCENELIGHTING3D_H
#define PINGPONG_SCENELIGHTING3D_H

#include "Core/Graphics/Rendering/Lighting/LightTypes3D.h"
#include "Core/Graphics/Rendering/Lighting/ShaderConstants3D.h"

#include <SimpleMath.h>

#include <vector>

struct SceneLightingDescriptor3D final
{
    std::vector<DirectionalLight3D> DirectionalLights{};
    std::vector<PointLight3D> PointLights{};
    std::vector<SpotLight3D> SpotLights{};
};

[[nodiscard]] DirectX::SimpleMath::Vector3 CameraWorldPositionFromViewMatrix(
    const DirectX::SimpleMath::Matrix &view
);

void FillLightsGpuConstants(
    const SceneLightingDescriptor3D &lighting,
    LightsGpuConstants &lightsGpuConstants
);

[[nodiscard]] inline SceneLightingDescriptor3D SceneLighting3DCreateDefaultOutdoor()
{
    SceneLightingDescriptor3D result{};
    DirectionalLight3D sun{};
    sun.Direction = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
    sun.LightColor = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f);
    sun.Intensity = 1.0f;
    sun.Enabled = true;
    result.DirectionalLights.push_back(sun);
    return result;
}

#endif
