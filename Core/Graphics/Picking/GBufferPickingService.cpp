#include "Core/Graphics/Picking/GBufferPickingService.h"

#include "Core/Graphics/Camera.h"
#include "Core/Graphics/D3d11Helpers.h"
#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Rendering/Deferred/GBufferLayout.h"
#include "Core/Graphics/Rendering/Deferred/GBufferResources.h"
#include "Core/Graphics/ShaderCompiler.h"

#include <Windows.h>

#include <cmath>
#include <cstring>
#include <filesystem>
#include <stdexcept>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

namespace
{
constexpr std::uint32_t kPickPackedUintCount = 10u;

struct GBufferPickGpuConstants
{
    Matrix InverseProjection{};
    Matrix InverseView{};
    std::int32_t PickPixelX{0};
    std::int32_t PickPixelY{0};
    std::int32_t GBufferTextureWidth{0};
    std::int32_t GBufferTextureHeight{0};
};
}

void GBufferPickingService::ReleaseGpuResources() noexcept
{
    m_pickResultUnorderedAccessView.Reset();
    m_pickResultStagingBuffer.Reset();
    m_pickResultGpuBuffer.Reset();
    m_pickConstantBuffer.Reset();
    m_pickComputeShader.Reset();
    m_gpuPickReady = false;
}

bool GBufferPickingService::TryInitializeGpuResources(ID3D11Device *const device)
{
    ReleaseGpuResources();
    if (device == nullptr)
    {
        return false;
    }

    try
    {
        const Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = ShaderCompiler::CompileFromFile(
            std::filesystem::path("Core/Shaders/GBufferPickResolve.hlsl"),
            "PickMain",
            "cs_5_0"
        );

        D3d11Helpers::ThrowIfFailed(
            device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, m_pickComputeShader.GetAddressOf()),
            "GBufferPickingService failed to create pick compute shader."
        );

        D3D11_BUFFER_DESC constantBufferDescription{};
        constantBufferDescription.ByteWidth = static_cast<UINT>(sizeof(GBufferPickGpuConstants));
        constantBufferDescription.Usage = D3D11_USAGE_DEFAULT;
        constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDescription.CPUAccessFlags = 0u;
        constantBufferDescription.MiscFlags = 0u;
        constantBufferDescription.StructureByteStride = 0u;

        D3d11Helpers::ThrowIfFailed(
            device->CreateBuffer(&constantBufferDescription, nullptr, m_pickConstantBuffer.GetAddressOf()),
            "GBufferPickingService failed to create pick constant buffer."
        );

        constexpr UINT kResultElementCount = 16u;
        D3D11_BUFFER_DESC resultGpuDescription{};
        resultGpuDescription.ByteWidth = sizeof(std::uint32_t) * kResultElementCount;
        resultGpuDescription.Usage = D3D11_USAGE_DEFAULT;
        resultGpuDescription.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        resultGpuDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        resultGpuDescription.StructureByteStride = sizeof(std::uint32_t);

        D3d11Helpers::ThrowIfFailed(
            device->CreateBuffer(&resultGpuDescription, nullptr, m_pickResultGpuBuffer.GetAddressOf()),
            "GBufferPickingService failed to create pick GPU result buffer."
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDescription{};
        unorderedAccessViewDescription.Format = DXGI_FORMAT_UNKNOWN;
        unorderedAccessViewDescription.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        unorderedAccessViewDescription.Buffer.FirstElement = 0u;
        unorderedAccessViewDescription.Buffer.NumElements = kResultElementCount;
        unorderedAccessViewDescription.Buffer.Flags = 0u;

        D3d11Helpers::ThrowIfFailed(
            device->CreateUnorderedAccessView(
                m_pickResultGpuBuffer.Get(),
                &unorderedAccessViewDescription,
                m_pickResultUnorderedAccessView.GetAddressOf()
            ),
            "GBufferPickingService failed to create pick result UAV."
        );

        D3D11_BUFFER_DESC stagingDescription{};
        stagingDescription.ByteWidth = sizeof(std::uint32_t) * kResultElementCount;
        stagingDescription.Usage = D3D11_USAGE_STAGING;
        stagingDescription.BindFlags = 0u;
        stagingDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        stagingDescription.MiscFlags = 0u;
        stagingDescription.StructureByteStride = 0u;

        D3d11Helpers::ThrowIfFailed(
            device->CreateBuffer(&stagingDescription, nullptr, m_pickResultStagingBuffer.GetAddressOf()),
            "GBufferPickingService failed to create pick staging buffer."
        );
    }
    catch (const std::exception &exception)
    {
        OutputDebugStringA(exception.what());
        ReleaseGpuResources();
        return false;
    }

    m_gpuPickReady = true;
    return true;
}

void GBufferPickingService::Initialize(GraphicsDevice &graphics)
{
    ID3D11Device *const device = graphics.GetDevice();
    if (device == nullptr)
    {
        return;
    }

    EnsureStagingTextures(device);
    TryInitializeGpuResources(device);
    m_initialized = true;
}

void GBufferPickingService::Shutdown() noexcept
{
    ReleaseGpuResources();
    m_stagingObjectId.Reset();
    m_stagingNormal.Reset();
    m_stagingDepth.Reset();
    m_initialized = false;
}

void GBufferPickingService::EnsureStagingTextures(ID3D11Device *const device)
{
    if (device == nullptr)
    {
        return;
    }

    auto createStaging1By1 = [&](const DXGI_FORMAT format, Microsoft::WRL::ComPtr<ID3D11Texture2D> &outTexture) {
        if (outTexture != nullptr)
        {
            D3D11_TEXTURE2D_DESC existing{};
            outTexture->GetDesc(&existing);
            if (existing.Format == format && existing.Width == 1u && existing.Height == 1u)
            {
                return;
            }
        }

        outTexture.Reset();
        D3D11_TEXTURE2D_DESC description{};
        description.Width = 1u;
        description.Height = 1u;
        description.MipLevels = 1u;
        description.ArraySize = 1u;
        description.Format = format;
        description.SampleDesc.Count = 1u;
        description.SampleDesc.Quality = 0u;
        description.Usage = D3D11_USAGE_STAGING;
        description.BindFlags = 0u;
        description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        description.MiscFlags = 0u;

        D3d11Helpers::ThrowIfFailed(
            device->CreateTexture2D(&description, nullptr, outTexture.GetAddressOf()),
            "GBufferPickingService failed to create staging texture."
        );
    };

    createStaging1By1(DXGI_FORMAT_R32_UINT, m_stagingObjectId);
    createStaging1By1(kGBufferNormalFormat, m_stagingNormal);
    createStaging1By1(DXGI_FORMAT_R32_FLOAT, m_stagingDepth);
}

GBufferPickResult GBufferPickingService::Pick(
    GraphicsDevice &graphics,
    const GBufferResources &gBuffer,
    const Camera &camera,
    const float viewportAspectRatio,
    const int screenX,
    const int screenY
)
{
    if (m_gpuPickReady)
    {
        return PickGpu(graphics, gBuffer, camera, viewportAspectRatio, screenX, screenY);
    }

    return PickCpu(graphics, gBuffer, camera, viewportAspectRatio, screenX, screenY);
}

GBufferPickResult GBufferPickingService::PickGpu(
    GraphicsDevice &graphics,
    const GBufferResources &gBuffer,
    const Camera &camera,
    const float viewportAspectRatio,
    const int screenX,
    const int screenY
)
{
    GBufferPickResult result{};
    result.ScreenX = screenX;
    result.ScreenY = screenY;

    if (!m_initialized || !gBuffer.IsCreated() || !m_gpuPickReady)
    {
        return PickCpu(graphics, gBuffer, camera, viewportAspectRatio, screenX, screenY);
    }

    ID3D11Device *const device = graphics.GetDevice();
    ID3D11DeviceContext *const context = graphics.GetImmediateContext();
    if (device == nullptr || context == nullptr
        || m_pickComputeShader == nullptr
        || m_pickConstantBuffer == nullptr
        || m_pickResultGpuBuffer == nullptr
        || m_pickResultStagingBuffer == nullptr
        || m_pickResultUnorderedAccessView == nullptr)
    {
        return PickCpu(graphics, gBuffer, camera, viewportAspectRatio, screenX, screenY);
    }

    const std::uint32_t textureWidth = gBuffer.GetWidth();
    const std::uint32_t textureHeight = gBuffer.GetHeight();
    if (textureWidth == 0u || textureHeight == 0u)
    {
        return result;
    }

    if (!(viewportAspectRatio > 0.0f) || std::isnan(viewportAspectRatio) || std::isinf(viewportAspectRatio))
    {
        return result;
    }

    if (screenX < 0 || screenY < 0
        || screenX >= static_cast<int>(textureWidth)
        || screenY >= static_cast<int>(textureHeight))
    {
        return result;
    }

    const Matrix viewMatrix = camera.GetViewMatrix();
    const Matrix projectionMatrix = camera.GetProjectionMatrix(viewportAspectRatio);
    const Matrix inverseProjection = projectionMatrix.Invert();
    const Matrix inverseView = viewMatrix.Invert();

    GBufferPickGpuConstants constants{};
    constants.InverseProjection = inverseProjection;
    constants.InverseView = inverseView;
    constants.PickPixelX = static_cast<std::int32_t>(screenX);
    constants.PickPixelY = static_cast<std::int32_t>(screenY);
    constants.GBufferTextureWidth = static_cast<std::int32_t>(textureWidth);
    constants.GBufferTextureHeight = static_cast<std::int32_t>(textureHeight);

    context->UpdateSubresource(m_pickConstantBuffer.Get(), 0u, nullptr, &constants, 0u, 0u);

    ID3D11ShaderResourceView *const shaderResourceViews[3] = {
        gBuffer.GetNormalTarget().GetShaderResourceView(),
        gBuffer.GetObjectIdTarget().GetShaderResourceView(),
        gBuffer.GetSceneDepthTarget().GetShaderResourceView()
    };

    ID3D11Buffer *const constantBuffers[1] = {m_pickConstantBuffer.Get()};
    context->CSSetConstantBuffers(0u, 1u, constantBuffers);
    context->CSSetShaderResources(0u, 3u, shaderResourceViews);

    ID3D11UnorderedAccessView *const unorderedAccessViews[1] = {m_pickResultUnorderedAccessView.Get()};
    context->CSSetUnorderedAccessViews(0u, 1u, unorderedAccessViews, nullptr);

    context->CSSetShader(m_pickComputeShader.Get(), nullptr, 0u);
    context->Dispatch(1u, 1u, 1u);

    ID3D11ShaderResourceView *const nullShaderResourceViews[3] = {nullptr, nullptr, nullptr};
    context->CSSetShaderResources(0u, 3u, nullShaderResourceViews);
    ID3D11UnorderedAccessView *const nullUnorderedAccessViews[1] = {nullptr};
    context->CSSetUnorderedAccessViews(0u, 1u, nullUnorderedAccessViews, nullptr);
    context->CSSetShader(nullptr, nullptr, 0u);

    context->CopyResource(m_pickResultStagingBuffer.Get(), m_pickResultGpuBuffer.Get());

    std::uint32_t packed[kPickPackedUintCount]{};
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        const HRESULT mapResult = context->Map(m_pickResultStagingBuffer.Get(), 0u, D3D11_MAP_READ, 0u, &mapped);
        if (FAILED(mapResult))
        {
            return PickCpu(graphics, gBuffer, camera, viewportAspectRatio, screenX, screenY);
        }
        std::memcpy(packed, mapped.pData, sizeof(std::uint32_t) * kPickPackedUintCount);
        context->Unmap(m_pickResultStagingBuffer.Get(), 0u);
    }

    result.ObjectId = packed[0u];
    result.Depth = *reinterpret_cast<const float *>(&packed[2u]);
    const std::uint32_t hitFlag = packed[1u];
    if (hitFlag == 0u || result.ObjectId == 0u || result.Depth >= 1.0f)
    {
        return result;
    }

    result.WorldNormal = Vector3(
        *reinterpret_cast<const float *>(&packed[3u]),
        *reinterpret_cast<const float *>(&packed[4u]),
        *reinterpret_cast<const float *>(&packed[5u])
    );
    result.WorldNormal.Normalize();
    result.WorldPosition = Vector3(
        *reinterpret_cast<const float *>(&packed[6u]),
        *reinterpret_cast<const float *>(&packed[7u]),
        *reinterpret_cast<const float *>(&packed[8u])
    );
    result.Hit = true;
    return result;
}

GBufferPickResult GBufferPickingService::PickCpu(
    GraphicsDevice &graphics,
    const GBufferResources &gBuffer,
    const Camera &camera,
    const float viewportAspectRatio,
    const int screenX,
    const int screenY
)
{
    GBufferPickResult result{};
    result.ScreenX = screenX;
    result.ScreenY = screenY;

    if (!m_initialized || !gBuffer.IsCreated())
    {
        return result;
    }

    ID3D11Device *const device = graphics.GetDevice();
    ID3D11DeviceContext *const context = graphics.GetImmediateContext();
    if (device == nullptr || context == nullptr)
    {
        return result;
    }

    const std::uint32_t textureWidth = gBuffer.GetWidth();
    const std::uint32_t textureHeight = gBuffer.GetHeight();
    if (textureWidth == 0u || textureHeight == 0u)
    {
        return result;
    }

    if (!(viewportAspectRatio > 0.0f) || std::isnan(viewportAspectRatio) || std::isinf(viewportAspectRatio))
    {
        return result;
    }

    if (screenX < 0 || screenY < 0
        || screenX >= static_cast<int>(textureWidth)
        || screenY >= static_cast<int>(textureHeight))
    {
        return result;
    }

    EnsureStagingTextures(device);

    if (m_stagingObjectId == nullptr || m_stagingNormal == nullptr || m_stagingDepth == nullptr)
    {
        return result;
    }

    D3D11_BOX sourceBox{};
    sourceBox.left = static_cast<std::uint32_t>(screenX);
    sourceBox.top = static_cast<std::uint32_t>(screenY);
    sourceBox.front = 0u;
    sourceBox.right = sourceBox.left + 1u;
    sourceBox.bottom = sourceBox.top + 1u;
    sourceBox.back = 1u;

    ID3D11Texture2D *const objectIdTexture = gBuffer.GetObjectIdTarget().GetTexture();
    ID3D11Texture2D *const normalTexture = gBuffer.GetNormalTarget().GetTexture();
    ID3D11Texture2D *const depthTexture = gBuffer.GetSceneDepthTarget().GetTexture();

    context->CopySubresourceRegion(m_stagingObjectId.Get(), 0u, 0, 0, 0, objectIdTexture, 0u, &sourceBox);
    context->CopySubresourceRegion(m_stagingNormal.Get(), 0u, 0, 0, 0, normalTexture, 0u, &sourceBox);
    context->CopySubresourceRegion(m_stagingDepth.Get(), 0u, 0, 0, 0, depthTexture, 0u, &sourceBox);

    std::uint32_t ObjectIdValue = 0u;
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        D3d11Helpers::ThrowIfFailed(
            context->Map(m_stagingObjectId.Get(), 0u, D3D11_MAP_READ, 0u, &mapped),
            "GBufferPickingService failed to map object id staging texture."
        );
        ObjectIdValue = *reinterpret_cast<const std::uint32_t *>(mapped.pData);
        context->Unmap(m_stagingObjectId.Get(), 0u);
    }

    std::uint32_t PackedNormal = 0u;
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        D3d11Helpers::ThrowIfFailed(
            context->Map(m_stagingNormal.Get(), 0u, D3D11_MAP_READ, 0u, &mapped),
            "GBufferPickingService failed to map normal staging texture."
        );
        PackedNormal = *reinterpret_cast<const std::uint32_t *>(mapped.pData);
        context->Unmap(m_stagingNormal.Get(), 0u);
    }

    float DeviceDepth = 0.0f;
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        D3d11Helpers::ThrowIfFailed(
            context->Map(m_stagingDepth.Get(), 0u, D3D11_MAP_READ, 0u, &mapped),
            "GBufferPickingService failed to map depth staging texture."
        );
        DeviceDepth = *reinterpret_cast<const float *>(mapped.pData);
        context->Unmap(m_stagingDepth.Get(), 0u);
    }

    result.ObjectId = ObjectIdValue;
    result.Depth = DeviceDepth;

    if (ObjectIdValue == 0u || DeviceDepth >= 1.0f)
    {
        return result;
    }

    constexpr float kInverseTenBit = 1.0f / 1023.0f;
    const float encodedNormalX = static_cast<float>(PackedNormal & 1023u) * kInverseTenBit;
    const float encodedNormalY = static_cast<float>((PackedNormal >> 10) & 1023u) * kInverseTenBit;
    const float encodedNormalZ = static_cast<float>((PackedNormal >> 20) & 1023u) * kInverseTenBit;
    Vector3 worldNormal(
        encodedNormalX * 2.0f - 1.0f,
        encodedNormalY * 2.0f - 1.0f,
        encodedNormalZ * 2.0f - 1.0f
    );
    worldNormal.Normalize();
    result.WorldNormal = worldNormal;

    const Matrix viewMatrix = camera.GetViewMatrix();
    const Matrix projectionMatrix = camera.GetProjectionMatrix(viewportAspectRatio);
    const Matrix inverseProjection = projectionMatrix.Invert();
    const Matrix inverseView = viewMatrix.Invert();

    const float normalizedDeviceCoordinateX =
        (static_cast<float>(screenX) + 0.5f) / static_cast<float>(textureWidth) * 2.0f - 1.0f;
    const float normalizedDeviceCoordinateY =
        1.0f - (static_cast<float>(screenY) + 0.5f) / static_cast<float>(textureHeight) * 2.0f;

    Vector4 clipPosition(normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, DeviceDepth, 1.0f);
    Vector4 viewPosition = Vector4::Transform(clipPosition, inverseProjection);
    const float viewW = std::fabs(viewPosition.w);
    if (viewW > 1.0e-5f)
    {
        viewPosition.x /= viewPosition.w;
        viewPosition.y /= viewPosition.w;
        viewPosition.z /= viewPosition.w;
    }

    Vector4 worldPosition4 = Vector4::Transform(
        Vector4(viewPosition.x, viewPosition.y, viewPosition.z, 1.0f),
        inverseView
    );
    const float worldW = std::fabs(worldPosition4.w);
    if (worldW > 1.0e-5f)
    {
        worldPosition4.x /= worldPosition4.w;
        worldPosition4.y /= worldPosition4.w;
        worldPosition4.z /= worldPosition4.w;
    }

    result.WorldPosition = Vector3(worldPosition4.x, worldPosition4.y, worldPosition4.z);
    result.Hit = true;
    return result;
}
