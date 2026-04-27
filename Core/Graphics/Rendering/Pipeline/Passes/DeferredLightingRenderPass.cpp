#include "Core/Graphics/Rendering/Pipeline/Passes/DeferredLightingRenderPass.h"
#include "Core/Gameplay/Scene.h"
#include "Core/Graphics/Camera.h"
#include "Core/Graphics/D3d11Helpers.h"
#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Rendering/Deferred/DeferredFrameResources.h"
#include "Core/Graphics/Rendering/Deferred/DeferredShaderConstants3D.h"
#include "Core/Graphics/Rendering/Deferred/GBufferResources.h"
#include "Core/Graphics/Rendering/Lighting/SceneLighting3D.h"
#include "Core/Graphics/Rendering/Lighting/ShaderConstants3D.h"
#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"
#include "Core/Graphics/Rendering/Pipeline/FrameRenderResources.h"
#include "Core/Graphics/Rendering/RenderContext.h"
#include "Core/Graphics/ShaderCompiler.h"

#include <cstddef>
#include <cstring>
#include <stdexcept>

namespace
{
    void CreateDynamicConstantBuffer(
        ID3D11Device *const device,
        const UINT byteWidth,
        Microsoft::WRL::ComPtr<ID3D11Buffer> &buffer,
        const char *const errorMessage
    )
    {
        D3D11_BUFFER_DESC description{};
        description.Usage = D3D11_USAGE_DYNAMIC;
        description.ByteWidth = byteWidth;
        description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3d11Helpers::ThrowIfFailed(
            device->CreateBuffer(&description, nullptr, buffer.GetAddressOf()),
            errorMessage
        );
    }

    void UploadConstantBuffer(
        ID3D11DeviceContext *const deviceContext,
        ID3D11Buffer *const constantBuffer,
        const void *const data,
        const std::size_t byteCount,
        const char *const errorMessage
    )
    {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        D3d11Helpers::ThrowIfFailed(
            deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped),
            errorMessage
        );
        std::memcpy(mapped.pData, data, byteCount);
        deviceContext->Unmap(constantBuffer, 0);
    }
}

void DeferredLightingRenderPass::Initialize(GraphicsDevice &graphicsDevice, FrameRenderResources &)
{
    ID3D11Device *const device = graphicsDevice.GetDevice();
    if (device == nullptr)
    {
        return;
    }

    const auto vertexShaderBlob = ShaderCompiler::CompileFromFile("Core/Shaders/DeferredLighting3D.hlsl", "VSMain", "vs_5_0");
    const auto pixelShaderBlob = ShaderCompiler::CompileFromFile("Core/Shaders/DeferredLighting3D.hlsl", "PSMain", "ps_5_0");

    D3d11Helpers::ThrowIfFailed(
        device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, VertexShader.GetAddressOf()),
        "DeferredLightingRenderPass failed to create vertex shader."
    );
    D3d11Helpers::ThrowIfFailed(
        device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, PixelShader.GetAddressOf()),
        "DeferredLightingRenderPass failed to create pixel shader."
    );

    D3D11_SAMPLER_DESC samplerDescription{};
    samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDescription.MinLOD = 0.0f;
    samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
    D3d11Helpers::ThrowIfFailed(
        device->CreateSamplerState(&samplerDescription, SamplerState.GetAddressOf()),
        "DeferredLightingRenderPass failed to create sampler state."
    );

    CreateDynamicConstantBuffer(
        device,
        static_cast<UINT>(sizeof(DeferredScreenConstantsGpu)),
        ScreenConstantBuffer,
        "DeferredLightingRenderPass failed to create screen constant buffer."
    );
    CreateDynamicConstantBuffer(
        device,
        static_cast<UINT>(sizeof(LightsGpuConstants)),
        LightsConstantBuffer,
        "DeferredLightingRenderPass failed to create lights constant buffer."
    );
}

void DeferredLightingRenderPass::Shutdown()
{
    LightsConstantBuffer.Reset();
    ScreenConstantBuffer.Reset();
    SamplerState.Reset();
    PixelShader.Reset();
    VertexShader.Reset();
}

void DeferredLightingRenderPass::Resize(int, int)
{
}

void DeferredLightingRenderPass::Execute(FramePassRenderContext &framePassRenderContext)
{
    DeferredFrameResources *const deferredFrameResources = framePassRenderContext.GetFrameRenderResources().GetDeferredFrameResources();
    if (deferredFrameResources == nullptr || !deferredFrameResources->IsCreated())
    {
        return;
    }

    Camera *const activeCamera = framePassRenderContext.GetActiveCamera();
    Scene *const gameplayScene = framePassRenderContext.GetGameplayScene();
    if (activeCamera == nullptr || gameplayScene == nullptr)
    {
        return;
    }

    GraphicsDevice &graphicsDevice = framePassRenderContext.GetGraphicsDevice();
    auto &lightAccumulationTarget = deferredFrameResources->GetLightAccumulationTarget();
    framePassRenderContext.GetRenderContext().GetFrameRenderer().BindDeferredLightingAccumulation(lightAccumulationTarget);
    lightAccumulationTarget.Clear(graphicsDevice, Color(0.0f, 0.0f, 0.0f, 1.0f));

    ID3D11DeviceContext *const deviceContext = graphicsDevice.GetImmediateContext();
    if (deviceContext == nullptr
        || VertexShader == nullptr
        || PixelShader == nullptr
        || ScreenConstantBuffer == nullptr
        || LightsConstantBuffer == nullptr)
    {
        return;
    }

    const float aspectRatio = framePassRenderContext.GetViewportAspectRatio();
    const DirectX::SimpleMath::Matrix viewMatrix = activeCamera->GetViewMatrix();
    const DirectX::SimpleMath::Matrix projectionMatrix = activeCamera->GetProjectionMatrix(aspectRatio);
    const DirectX::SimpleMath::Vector3 cameraWorldPosition = CameraWorldPositionFromViewMatrix(viewMatrix);

    DeferredScreenConstantsGpu screenConstants{};
    screenConstants.InverseProjection = projectionMatrix.Invert().Transpose();
    screenConstants.InverseView = viewMatrix.Invert().Transpose();
    screenConstants.CameraWorldPosition = DirectX::XMFLOAT4(
        cameraWorldPosition.x,
        cameraWorldPosition.y,
        cameraWorldPosition.z,
        1.0f
    );
    screenConstants.ScreenInverseWidthHeightAndFrameIndex = DirectX::XMFLOAT4(
        lightAccumulationTarget.GetWidth() > 0u ? 1.0f / static_cast<float>(lightAccumulationTarget.GetWidth()) : 0.0f,
        lightAccumulationTarget.GetHeight() > 0u ? 1.0f / static_cast<float>(lightAccumulationTarget.GetHeight()) : 0.0f,
        0.0f,
        0.0f
    );

    LightsGpuConstants lightsConstants{};
    FillLightsGpuConstants(gameplayScene->GetSceneLightingDescriptor(), lightsConstants);

    UploadConstantBuffer(
        deviceContext,
        ScreenConstantBuffer.Get(),
        &screenConstants,
        sizeof(DeferredScreenConstantsGpu),
        "DeferredLightingRenderPass failed to map screen constant buffer."
    );
    UploadConstantBuffer(
        deviceContext,
        LightsConstantBuffer.Get(),
        &lightsConstants,
        sizeof(LightsGpuConstants),
        "DeferredLightingRenderPass failed to map lights constant buffer."
    );

    ID3D11Buffer *const screenConstantBuffers[] = {ScreenConstantBuffer.Get()};
    ID3D11Buffer *const lightsConstantBuffers[] = {LightsConstantBuffer.Get()};
    deviceContext->VSSetConstantBuffers(0, 1, screenConstantBuffers);
    deviceContext->PSSetConstantBuffers(0, 1, screenConstantBuffers);
    deviceContext->PSSetConstantBuffers(3, 1, lightsConstantBuffers);

    GBufferResources &gBuffer = deferredFrameResources->GetGBufferResources();
    ID3D11ShaderResourceView *const shaderResourceViews[] =
    {
        gBuffer.GetAlbedoOcclusionTarget().GetShaderResourceView(),
        gBuffer.GetNormalTarget().GetShaderResourceView(),
        gBuffer.GetMaterialTarget().GetShaderResourceView(),
        gBuffer.GetEmissiveTarget().GetShaderResourceView(),
        gBuffer.GetSceneDepthTarget().GetShaderResourceView()
    };
    deviceContext->PSSetShaderResources(0, 5, shaderResourceViews);
    framePassRenderContext.GetRenderContext().BindDirectionalShadowRegisters(deviceContext, 5u, 1u);

    ID3D11SamplerState *const samplers[] = {SamplerState.Get()};
    deviceContext->PSSetSamplers(0, 1, samplers);
    deviceContext->IASetInputLayout(nullptr);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->VSSetShader(VertexShader.Get(), nullptr, 0);
    deviceContext->PSSetShader(PixelShader.Get(), nullptr, 0);
    deviceContext->Draw(3, 0);

    ID3D11ShaderResourceView *const nullShaderResourceViews[6] = {};
    deviceContext->PSSetShaderResources(0, 6, nullShaderResourceViews);
    framePassRenderContext.GetRenderContext().UnbindDirectionalShadowShaderResource(deviceContext, 5u);
}
