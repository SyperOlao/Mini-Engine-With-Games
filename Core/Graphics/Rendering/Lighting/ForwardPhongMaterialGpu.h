#ifndef PINGPONG_FORWARDPHONGMATERIALGPU_H
#define PINGPONG_FORWARDPHONGMATERIALGPU_H

#include "Core/Graphics/Rendering/Lighting/ShaderConstants3D.h"
#include "Core/Graphics/Rendering/Renderables/RenderMaterialParameters.h"

#include <DirectXMath.h>

inline void FillMaterialGpuConstantsFromRenderMaterial(
    const RenderMaterialParameters &material,
    MaterialGpuConstants &destination
)
{
    const DirectX::XMFLOAT4 &baseColorFloat4 = static_cast<const DirectX::XMFLOAT4 &>(material.BaseColor);
    destination.BaseColor = baseColorFloat4;
    destination.SpecularColorAndPower = DirectX::XMFLOAT4(
        material.SpecularColor.x,
        material.SpecularColor.y,
        material.SpecularColor.z,
        material.SpecularPower
    );
    destination.EmissiveAndAmbient = DirectX::XMFLOAT4(
        material.EmissiveColor.x,
        material.EmissiveColor.y,
        material.EmissiveColor.z,
        material.AmbientFactor
    );
    destination.MaterialParameters = DirectX::XMFLOAT4(
        material.ReceiveLighting ? 1.0f : 0.0f,
        0.0f,
        0.0f,
        0.0f
    );
}

#endif
