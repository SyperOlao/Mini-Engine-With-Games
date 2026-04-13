#include "Core/Graphics/Rendering/Pipeline/Passes/MainGeometryBindRenderPass.h"

#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"
#include "Core/Graphics/Rendering/Pipeline/FrameRenderResources.h"
#include "Core/Graphics/Rendering/RenderContext.h"

void MainGeometryBindRenderPass::Initialize(GraphicsDevice &, FrameRenderResources &)
{
}

void MainGeometryBindRenderPass::Shutdown()
{
}

void MainGeometryBindRenderPass::Resize(int, int)
{
}

void MainGeometryBindRenderPass::Execute(FramePassRenderContext &framePassRenderContext)
{
    framePassRenderContext.GetRenderContext().GetFrameRenderer().BindMainPassTargets();
    framePassRenderContext.GetGraphicsDevice().ClearMainDepthStencil();
}
