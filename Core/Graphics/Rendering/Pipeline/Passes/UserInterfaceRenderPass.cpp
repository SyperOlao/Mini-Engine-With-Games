#include "Core/Graphics/Rendering/Pipeline/Passes/UserInterfaceRenderPass.h"

#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"
#include "Core/Graphics/Rendering/Pipeline/FrameRenderResources.h"
#include "Core/Graphics/Rendering/RenderContext.h"
#include "Core/UI/ImGui/ImGuiLayer.h"

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

    if (ImGuiLayer::IsInitialized())
    {
        ImGuiLayer::BeginFrame();
    }

    if (renderContext.ShouldExecuteGameRenderCallbackDuringUserInterfacePass())
    {
        framePassRenderContext.ExecuteGameRenderCallback();
    }

    if (ImGuiLayer::IsInitialized())
    {
        ImGuiLayer::RenderDrawData();
    }

    renderContext.GetFrameRenderer().LeavePass();
}
