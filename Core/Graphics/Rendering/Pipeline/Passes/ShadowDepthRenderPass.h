#ifndef PINGPONG_SHADOWDEPTHRENDERPASS_H
#define PINGPONG_SHADOWDEPTHRENDERPASS_H

#include "Core/Graphics/Rendering/Pipeline/IRenderPass.h"

class ShadowDepthRenderPass final : public IRenderPass
{
public:
    void Initialize(GraphicsDevice &graphicsDevice, FrameRenderResources &frameRenderResources) override;

    void Shutdown() override;

    void Resize(int width, int height) override;

    void Execute(FramePassRenderContext &framePassRenderContext) override;
};

#endif
