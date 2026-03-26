#include "RenderContext.h"

#include "Core/Gameplay/Entity.h"
#include "Core/Gameplay/GameplayComponents.h"
#include "Core/Gameplay/Scene.h"
#include "Core/Graphics/Camera.h"
#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/ModelAsset.h"
#include "Core/Graphics/Rendering/FrameRenderTypes.h"
#include "Core/Graphics/Rendering/Lighting/LightTypes3D.h"
#include "Core/Graphics/Rendering/Lighting/SceneLighting3D.h"
#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"
#include "Core/Graphics/Rendering/Pipeline/Passes/DebugOverlayRenderPass.h"
#include "Core/Graphics/Rendering/Pipeline/Passes/MainGeometryBindRenderPass.h"
#include "Core/Graphics/Rendering/Pipeline/Passes/ShadowDepthRenderPass.h"
#include "Core/Graphics/Rendering/Pipeline/Passes/UserInterfaceRenderPass.h"
#include "Core/Graphics/Rendering/Shadows/ShadowProjection.h"

#include <memory>

RenderContext::~RenderContext()
{
    m_directionalShadowResources.Shutdown();
}

void RenderContext::Initialize(GraphicsDevice &graphics)
{
    m_graphics = &graphics;

    m_frameRenderResources.Initialize(graphics);
    m_frameRenderResources.SetDeferredFrameResources(&m_deferredFrameResources);

    m_frameRenderer.Initialize(graphics);
    m_deferredFrameResources.CreateOrResize(graphics);
    m_directionalShadowResources.Initialize(graphics);
    m_shapeRenderer2D.Initialize(graphics);
    m_primitiveRenderer3D.Initialize(graphics);
    m_primitiveRenderer3D.SetShadowBindingHost(this);
    m_modelRenderer.Initialize(graphics);
    m_modelRenderer.SetShadowBindingHost(this);
    m_sceneRenderer3D.Initialize(m_primitiveRenderer3D, m_modelRenderer);

    m_frameRenderPipeline.Initialize(graphics, m_frameRenderResources);
}

void RenderContext::BeginRenderFrame()
{
    m_directionalShadowPassCompletedThisFrame = false;
    InvalidateDirectionalShadowPass();
}

void RenderContext::PrepareDirectionalShadowPass(Scene &scene, Camera &camera)
{
    if (m_graphics == nullptr)
    {
        return;
    }

    if (m_directionalShadowPassCompletedThisFrame)
    {
        return;
    }
    m_directionalShadowPassCompletedThisFrame = true;

    ID3D11DeviceContext *const deviceContext = m_graphics->GetImmediateContext();
    if (deviceContext == nullptr)
    {
        return;
    }

    if (!scene.GetForwardLightingEnabled() || !scene.GetDirectionalShadowMappingEnabled())
    {
        InvalidateDirectionalShadowPass();
        return;
    }

    const SceneLightingDescriptor3D &lighting = scene.GetSceneLightingDescriptor();
    const DirectionalLight3D *shadowDirectional = nullptr;
    for (const DirectionalLight3D &directional : lighting.DirectionalLights)
    {
        if (directional.Enabled)
        {
            shadowDirectional = &directional;
            break;
        }
    }

    if (shadowDirectional == nullptr)
    {
        InvalidateDirectionalShadowPass();
        return;
    }

    std::uint32_t shadowPackedDirectionalIndex = 0u;
    for (const DirectionalLight3D &directional : lighting.DirectionalLights)
    {
        if (!directional.Enabled)
        {
            continue;
        }
        if (&directional == shadowDirectional)
        {
            break;
        }
        shadowPackedDirectionalIndex++;
    }

    DirectX::SimpleMath::Vector3 lightAxis = shadowDirectional->Direction;
    lightAxis.Normalize();

    const DirectX::SimpleMath::Matrix viewMatrix = camera.GetViewMatrix();
    const DirectX::SimpleMath::Vector3 focusPoint = CameraWorldPositionFromViewMatrix(viewMatrix);

    const DirectX::SimpleMath::Matrix lightViewProjection = ShadowProjection::BuildDirectionalLightViewProjection(
        lightAxis,
        focusPoint,
        DirectX::SimpleMath::Vector3::UnitY,
        m_directionalShadowResources.GetLightEyeDistanceFromFocus(),
        m_directionalShadowResources.GetOrthographicWidth(),
        m_directionalShadowResources.GetOrthographicHeight(),
        m_directionalShadowResources.GetNearPlaneDistance(),
        m_directionalShadowResources.GetFarPlaneDistance()
    );

    ShadowCascadeConstantsGpu cascadeCpu{};
    cascadeCpu.LightViewProjection[0] = lightViewProjection.Transpose();
    cascadeCpu.CascadeSplits.x = m_directionalShadowResources.GetFarPlaneDistance();
    cascadeCpu.CascadeCount = 1u;

    const float inverseWidth = 1.0f / static_cast<float>(m_directionalShadowResources.GetShadowMap().GetWidth());
    const float inverseHeight = 1.0f / static_cast<float>(m_directionalShadowResources.GetShadowMap().GetHeight());

    ShadowSamplingGpuConstants samplingCpu{};
    samplingCpu.DepthBiasAndPcfKernel = DirectX::XMFLOAT4(0.00035f, 2.5f, 0.025f, 2.0f);
    samplingCpu.InvShadowMapTexelSize = DirectX::XMFLOAT2(inverseWidth, inverseHeight);
    samplingCpu.ShadowEnabled = 1u;
    samplingCpu.ShadowedDirectionalLightGpuIndex = shadowPackedDirectionalIndex;
    samplingCpu.CascadeCount = 1u;

    m_directionalShadowResources.UploadShadowCascadeConstants(deviceContext, cascadeCpu);
    m_directionalShadowResources.UploadShadowSamplingConstants(deviceContext, samplingCpu);

    ID3D11ShaderResourceView *const nullShaderResourceViews[1] = {nullptr};
    deviceContext->PSSetShaderResources(0, 1, nullShaderResourceViews);

    m_frameRenderer.EnterPass(RenderPassKind::ShadowCapture);

    DepthRenderTarget2D &shadowTarget = m_directionalShadowResources.GetShadowMap();
    ID3D11DepthStencilView *const shadowDepthStencilView = shadowTarget.GetDepthStencilView();

    m_graphics->BindDepthOnlyRenderTarget(shadowDepthStencilView);
    m_graphics->SetViewportPixels(
        static_cast<float>(shadowTarget.GetWidth()),
        static_cast<float>(shadowTarget.GetHeight())
    );
    m_graphics->ClearDepthStencilView(shadowDepthStencilView, 1.0f, 0u);

    ID3D11RasterizerState *const shadowRasterizerState = m_directionalShadowResources.GetShadowPassRasterizerState();

    scene.ForEachEntityWithTransformAndModel([&](Entity &entity) {
        ModelComponent *const model = entity.TryGetModelComponent();
        TransformComponent *const transform = entity.TryGetTransformComponent();
        if (model == nullptr || transform == nullptr)
        {
            return;
        }
        if (!model->Visible || !model->CastsShadow || model->Asset == nullptr)
        {
            return;
        }

        m_modelRenderer.DrawModelShadowDepth(
            shadowRasterizerState,
            *model->Asset,
            transform->WorldMatrix,
            lightViewProjection
        );
    });

    deviceContext->RSSetState(nullptr);
    m_graphics->BindMainRenderTargets();
    m_graphics->SetMainViewport();
    m_frameRenderer.LeavePass();
}

void RenderContext::InvalidateDirectionalShadowPass()
{
    if (m_graphics == nullptr)
    {
        return;
    }

    ID3D11DeviceContext *const deviceContext = m_graphics->GetImmediateContext();
    if (deviceContext == nullptr)
    {
        return;
    }

    ShadowSamplingGpuConstants samplingCpu{};
    samplingCpu.ShadowEnabled = 0u;
    samplingCpu.CascadeCount = 0u;
    m_directionalShadowResources.UploadShadowSamplingConstants(deviceContext, samplingCpu);

    ShadowCascadeConstantsGpu cascadeCpu{};
    cascadeCpu.CascadeCount = 0u;
    m_directionalShadowResources.UploadShadowCascadeConstants(deviceContext, cascadeCpu);
}

void RenderContext::BindForwardPhongShadowRegisters(ID3D11DeviceContext *const deviceContext) const
{
    if (deviceContext == nullptr)
    {
        return;
    }

    ID3D11Buffer *const shadowConstantBuffers[] =
    {
        m_directionalShadowResources.GetShadowCascadeConstantBuffer(),
        m_directionalShadowResources.GetShadowSamplingConstantBuffer()
    };
    deviceContext->PSSetConstantBuffers(4, 2, shadowConstantBuffers);

    ID3D11ShaderResourceView *const shadowMapShaderResourceViews[] =
    {
        m_directionalShadowResources.GetShadowMap().GetShaderResourceView()
    };
    deviceContext->PSSetShaderResources(0, 1, shadowMapShaderResourceViews);

    ID3D11SamplerState *const shadowSamplers[] =
    {
        m_directionalShadowResources.GetShadowComparisonSampler()
    };
    deviceContext->PSSetSamplers(0, 1, shadowSamplers);
}

void RenderContext::UnbindForwardPhongShadowShaderResource(ID3D11DeviceContext *const deviceContext) const
{
    if (deviceContext == nullptr)
    {
        return;
    }

    ID3D11ShaderResourceView *const nullShaderResourceViews[1] = {nullptr};
    deviceContext->PSSetShaderResources(0, 1, nullShaderResourceViews);
}

void RenderContext::ResizeDeferredResources()
{
    if (m_graphics == nullptr)
    {
        return;
    }

    m_deferredFrameResources.CreateOrResize(*m_graphics);
    m_directionalShadowResources.Resize(*m_graphics);
    m_frameRenderResources.OnBackbufferResize(m_graphics->GetWidth(), m_graphics->GetHeight());
    m_frameRenderPipeline.Resize(m_graphics->GetWidth(), m_graphics->GetHeight());
}

ShapeRenderer2D &RenderContext::GetShapeRenderer2D() noexcept
{
    return m_shapeRenderer2D;
}

const ShapeRenderer2D &RenderContext::GetShapeRenderer2D() const noexcept
{
    return m_shapeRenderer2D;
}

PrimitiveRenderer3D &RenderContext::GetPrimitiveRenderer3D() noexcept
{
    return m_primitiveRenderer3D;
}

const PrimitiveRenderer3D &RenderContext::GetPrimitiveRenderer3D() const noexcept
{
    return m_primitiveRenderer3D;
}

SceneRenderer3D &RenderContext::GetSceneRenderer3D() noexcept
{
    return m_sceneRenderer3D;
}

const SceneRenderer3D &RenderContext::GetSceneRenderer3D() const noexcept
{
    return m_sceneRenderer3D;
}

ModelRenderer &RenderContext::GetModelRenderer() noexcept
{
    return m_modelRenderer;
}

const ModelRenderer &RenderContext::GetModelRenderer() const noexcept
{
    return m_modelRenderer;
}

DebugDrawQueue &RenderContext::GetDebugDraw() noexcept
{
    return m_debugDraw;
}

const DebugDrawQueue &RenderContext::GetDebugDraw() const noexcept
{
    return m_debugDraw;
}

FrameRenderer &RenderContext::GetFrameRenderer() noexcept
{
    return m_frameRenderer;
}

const FrameRenderer &RenderContext::GetFrameRenderer() const noexcept
{
    return m_frameRenderer;
}

DeferredFrameResources &RenderContext::GetDeferredFrameResources() noexcept
{
    return m_deferredFrameResources;
}

const DeferredFrameResources &RenderContext::GetDeferredFrameResources() const noexcept
{
    return m_deferredFrameResources;
}

FrameRenderResources &RenderContext::GetFrameRenderResources() noexcept
{
    return m_frameRenderResources;
}

const FrameRenderResources &RenderContext::GetFrameRenderResources() const noexcept
{
    return m_frameRenderResources;
}

FrameRenderPipeline &RenderContext::GetFrameRenderPipeline() noexcept
{
    return m_frameRenderPipeline;
}

const FrameRenderPipeline &RenderContext::GetFrameRenderPipeline() const noexcept
{
    return m_frameRenderPipeline;
}

void RenderContext::BuildDefaultForwardRenderPipeline()
{
    if (m_graphics == nullptr)
    {
        return;
    }

    m_frameRenderPipeline.ClearPasses();
    m_frameRenderPipeline.AddPass(std::make_unique<ShadowDepthRenderPass>());
    m_frameRenderPipeline.AddPass(std::make_unique<MainGeometryBindRenderPass>());
    m_frameRenderPipeline.AddPass(std::make_unique<DebugOverlayRenderPass>());
    m_frameRenderPipeline.AddPass(std::make_unique<UserInterfaceRenderPass>());
    m_frameRenderPipeline.Initialize(*m_graphics, m_frameRenderResources);
}

void RenderContext::ExecuteFramePipeline(
    Camera *const activeCamera,
    Scene *const gameplayScene,
    const float frameDeltaTimeSeconds
)
{
    FramePassRenderContext framePassRenderContext(
        *this,
        m_frameRenderResources,
        activeCamera,
        gameplayScene,
        static_cast<float>(m_frameRenderResources.GetBackbufferWidth()),
        static_cast<float>(m_frameRenderResources.GetBackbufferHeight()),
        frameDeltaTimeSeconds
    );
    m_frameRenderPipeline.ExecuteFrame(framePassRenderContext);
}
