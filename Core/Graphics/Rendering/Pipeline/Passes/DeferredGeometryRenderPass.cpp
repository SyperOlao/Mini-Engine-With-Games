#include "Core/Graphics/Rendering/Pipeline/Passes/DeferredGeometryRenderPass.h"
#include "Core/Graphics/Rendering/Deferred/DeferredFrameResources.h"
#include "Core/Graphics/Rendering/Deferred/GBufferResources.h"
#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"
#include "Core/Graphics/Rendering/Pipeline/FrameRenderResources.h"
#include "Core/Graphics/Rendering/RenderContext.h"

void DeferredGeometryRenderPass::Initialize(GraphicsDevice &, FrameRenderResources &)
{
}

void DeferredGeometryRenderPass::Shutdown()
{
}

void DeferredGeometryRenderPass::Resize(int, int)
{
}

void DeferredGeometryRenderPass::Execute(FramePassRenderContext &framePassRenderContext)
{
    DeferredFrameResources *const deferredFrameResources = framePassRenderContext.GetFrameRenderResources().GetDeferredFrameResources();
    if (deferredFrameResources == nullptr || !deferredFrameResources->IsCreated())
    {
        return;
    }

    GBufferResources &gBufferResources = deferredFrameResources->GetGBufferResources();
    framePassRenderContext.GetRenderContext().GetFrameRenderer().BindDeferredGeometryPass(gBufferResources);
    gBufferResources.ClearGeometryPassTargets(
        framePassRenderContext.GetGraphicsDevice(),
        Color(0.0f, 0.0f, 0.0f, 1.0f),
        Color(0.5f, 0.5f, 1.0f, 1.0f),
        Color(0.0f, 0.0f, 0.0f, 1.0f),
        Color(0.0f, 0.0f, 0.0f, 1.0f),
        1.0f
    );
}
