#include "Core/Graphics/Rendering/Pipeline/Passes/UserInterfaceRenderPass.h"

#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"
#include "Core/Graphics/Rendering/Pipeline/FrameRenderResources.h"
#include "Core/Graphics/Rendering/RenderContext.h"

void UserInterfaceRenderPass::Initialize(GraphicsDevice &, FrameRenderResources &)
{
}

void UserInterfaceRenderPass::Shutdown()
{
}

void UserInterfaceRenderPass::Resize(int, int)
{
}

void UserInterfaceRenderPass::Execute(FramePassRenderContext &framePassRenderContext)
{
    RenderContext &renderContext = framePassRenderContext.GetRenderContext();
    renderContext.GetFrameRenderer().BindMainPassTargets();
    renderContext.GetFrameRenderer().EnterPass(RenderPassKind::UserInterface);

    if (renderContext.ShouldExecuteGameRenderCallbackDuringUserInterfacePass())
    {
        framePassRenderContext.ExecuteGameRenderCallback();
    }

    renderContext.GetFrameRenderer().LeavePass();
}
