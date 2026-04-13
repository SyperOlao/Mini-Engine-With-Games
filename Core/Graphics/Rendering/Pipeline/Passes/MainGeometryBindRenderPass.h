#ifndef PINGPONG_MAINGEOMETRYBINDRENDERPASS_H
#define PINGPONG_MAINGEOMETRYBINDRENDERPASS_H

#include "Core/Graphics/Rendering/Pipeline/IRenderPass.h"

class MainGeometryBindRenderPass final : public IRenderPass
{
public:
    void Initialize(GraphicsDevice &graphicsDevice, FrameRenderResources &frameRenderResources) override;

    void Shutdown() override;

    void Resize(int width, int height) override;

    void Execute(FramePassRenderContext &framePassRenderContext) override;
};

#endif
