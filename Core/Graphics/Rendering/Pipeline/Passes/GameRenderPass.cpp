#include "Core/Graphics/Rendering/Pipeline/Passes/GameRenderPass.h"
#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"

void GameRenderPass::Initialize(GraphicsDevice &, FrameRenderResources &)
{
}

void GameRenderPass::Shutdown()
{
}

void GameRenderPass::Resize(int, int)
{
}

void GameRenderPass::Execute(FramePassRenderContext &framePassRenderContext)
{
    framePassRenderContext.ExecuteGameRenderCallback();
}
