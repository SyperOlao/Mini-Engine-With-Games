#include "Core/Graphics/Rendering/Pipeline/Passes/ShadowDepthRenderPass.h"

#include "Core/Gameplay/Scene.h"
#include "Core/Graphics/Camera.h"
#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Graphics/Rendering/Pipeline/FramePassRenderContext.h"
#include "Core/Graphics/Rendering/Pipeline/FrameRenderResources.h"
#include "Core/Graphics/Rendering/RenderContext.h"

void ShadowDepthRenderPass::Initialize(GraphicsDevice &, FrameRenderResources &)
{
}

void ShadowDepthRenderPass::Shutdown()
{
}

void ShadowDepthRenderPass::Resize(int, int)
{
}

void ShadowDepthRenderPass::Execute(FramePassRenderContext &framePassRenderContext)
{
    Scene *const gameplayScene = framePassRenderContext.GetGameplayScene();
    Camera *const activeCamera = framePassRenderContext.GetActiveCamera();
    if (gameplayScene == nullptr || activeCamera == nullptr)
    {
        return;
    }

    framePassRenderContext.GetRenderContext().PrepareDirectionalShadowPass(*gameplayScene, *activeCamera);
}
