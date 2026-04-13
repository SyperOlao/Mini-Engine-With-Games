#include "Core/Graphics/Rendering/ForwardPhongGpuUpload.h"

#include "Core/Graphics/D3d11Helpers.h"

#include <cstring>
#include <stdexcept>

void ForwardPhongUploadConstantBuffers(
    ID3D11DeviceContext *context,
    const CameraGpuConstants &camera,
    const ObjectGpuConstants &object,
    const MaterialGpuConstants &material,
    const LightsGpuConstants &lights,
    ID3D11Buffer *cameraBuffer,
    ID3D11Buffer *objectBuffer,
    ID3D11Buffer *materialBuffer,
    ID3D11Buffer *lightsBuffer
)
{
    if (context == nullptr)
    {
        throw std::logic_error("ForwardPhongUploadConstantBuffers requires a device context.");
    }

    if (cameraBuffer == nullptr || objectBuffer == nullptr || materialBuffer == nullptr || lightsBuffer == nullptr)
    {
        throw std::logic_error("ForwardPhongUploadConstantBuffers requires all constant buffers.");
    }

    D3D11_MAPPED_SUBRESOURCE mappedCamera{};
    D3d11Helpers::ThrowIfFailed(
        context->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCamera),
        "ForwardPhongUploadConstantBuffers failed to map camera constant buffer."
    );
    std::memcpy(mappedCamera.pData, &camera, sizeof(CameraGpuConstants));
    context->Unmap(cameraBuffer, 0);

    D3D11_MAPPED_SUBRESOURCE mappedObject{};
    D3d11Helpers::ThrowIfFailed(
        context->Map(objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedObject),
        "ForwardPhongUploadConstantBuffers failed to map object constant buffer."
    );
    std::memcpy(mappedObject.pData, &object, sizeof(ObjectGpuConstants));
    context->Unmap(objectBuffer, 0);

    D3D11_MAPPED_SUBRESOURCE mappedMaterial{};
    D3d11Helpers::ThrowIfFailed(
        context->Map(materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMaterial),
        "ForwardPhongUploadConstantBuffers failed to map material constant buffer."
    );
    std::memcpy(mappedMaterial.pData, &material, sizeof(MaterialGpuConstants));
    context->Unmap(materialBuffer, 0);

    D3D11_MAPPED_SUBRESOURCE mappedLights{};
    D3d11Helpers::ThrowIfFailed(
        context->Map(lightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedLights),
        "ForwardPhongUploadConstantBuffers failed to map lights constant buffer."
    );
    std::memcpy(mappedLights.pData, &lights, sizeof(LightsGpuConstants));
    context->Unmap(lightsBuffer, 0);
}
