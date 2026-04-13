#ifndef PINGPONG_DEBUGOVERLAYRENDERPASS_H
#define PINGPONG_DEBUGOVERLAYRENDERPASS_H

#include "Core/Graphics/Rendering/Pipeline/IRenderPass.h"

class DebugOverlayRenderPass final : public IRenderPass
{
public:
    void Initialize(GraphicsDevice &graphicsDevice, FrameRenderResources &frameRenderResources) override;

    void Shutdown() override;

    void Resize(int width, int height) override;

    void Execute(FramePassRenderContext &framePassRenderContext) override;
};

#endif
