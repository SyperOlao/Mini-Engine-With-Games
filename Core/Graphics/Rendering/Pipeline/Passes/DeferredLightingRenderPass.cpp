#include "Core/Graphics/Rendering/Pipeline/Passes/DeferredLightingRenderPass.h"
#include "Core/Graphics/D3d11Helpers.h"
#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Rendering/Deferred/DeferredFrameResources.h"
#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"
#include "Core/Graphics/Rendering/Pipeline/FrameRenderResources.h"
#include "Core/Graphics/Rendering/RenderContext.h"
#include "Core/Graphics/ShaderCompiler.h"

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
    samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDescription.MinLOD = 0.0f;
    samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
    D3d11Helpers::ThrowIfFailed(
        device->CreateSamplerState(&samplerDescription, SamplerState.GetAddressOf()),
        "DeferredLightingRenderPass failed to create sampler state."
    );
}

void DeferredLightingRenderPass::Shutdown()
{
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

    GraphicsDevice &graphicsDevice = framePassRenderContext.GetGraphicsDevice();
    auto &lightAccumulationTarget = deferredFrameResources->GetLightAccumulationTarget();
    framePassRenderContext.GetRenderContext().GetFrameRenderer().BindDeferredLightingAccumulation(lightAccumulationTarget);
    lightAccumulationTarget.Clear(graphicsDevice, Color(0.0f, 0.0f, 0.0f, 1.0f));

    ID3D11DeviceContext *const deviceContext = graphicsDevice.GetImmediateContext();
    if (deviceContext == nullptr || VertexShader == nullptr || PixelShader == nullptr)
    {
        return;
    }

    ID3D11ShaderResourceView *const albedoShaderResourceView = deferredFrameResources->GetGBufferResources().GetAlbedoOcclusionTarget().GetShaderResourceView();
    ID3D11ShaderResourceView *const shaderResourceViews[] = {albedoShaderResourceView};
    deviceContext->PSSetShaderResources(0, 1, shaderResourceViews);

    ID3D11SamplerState *const samplers[] = {SamplerState.Get()};
    deviceContext->PSSetSamplers(0, 1, samplers);
    deviceContext->IASetInputLayout(nullptr);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->VSSetShader(VertexShader.Get(), nullptr, 0);
    deviceContext->PSSetShader(PixelShader.Get(), nullptr, 0);
    deviceContext->Draw(3, 0);

    ID3D11ShaderResourceView *const nullShaderResourceViews[] = {nullptr};
    deviceContext->PSSetShaderResources(0, 1, nullShaderResourceViews);
}
