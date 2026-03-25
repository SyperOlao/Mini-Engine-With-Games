#include "RenderContext.h"

#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"
#include "Core/Graphics/Rendering/Pipeline/Passes/DebugOverlayRenderPass.h"
#include "Core/Graphics/Rendering/Pipeline/Passes/MainGeometryBindRenderPass.h"
#include "Core/Graphics/Rendering/Pipeline/Passes/ShadowDepthRenderPass.h"
#include "Core/Graphics/Rendering/Pipeline/Passes/UserInterfaceRenderPass.h"

#include <memory>

void RenderContext::Initialize(GraphicsDevice &graphics)
{
    m_graphics = &graphics;

    m_frameRenderResources.Initialize(graphics);
    m_frameRenderResources.SetDeferredFrameResources(&m_deferredFrameResources);

    m_frameRenderer.Initialize(graphics);
    m_deferredFrameResources.CreateOrResize(graphics);
    m_shapeRenderer2D.Initialize(graphics);
    m_primitiveRenderer3D.Initialize(graphics);
    m_modelRenderer.Initialize(graphics);
    m_sceneRenderer3D.Initialize(m_primitiveRenderer3D, m_modelRenderer);

    m_frameRenderPipeline.Initialize(graphics, m_frameRenderResources);
}

void RenderContext::ResizeDeferredResources()
{
    if (m_graphics == nullptr)
    {
        return;
    }

    m_deferredFrameResources.CreateOrResize(*m_graphics);
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
