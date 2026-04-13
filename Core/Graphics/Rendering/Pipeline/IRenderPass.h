#ifndef PINGPONG_IRENDERPASS_H
#define PINGPONG_IRENDERPASS_H

class FramePassRenderContext;
class FrameRenderResources;
class GraphicsDevice;

class IRenderPass
{
public:
    virtual ~IRenderPass() = default;

    virtual void Initialize(GraphicsDevice &graphicsDevice, FrameRenderResources &frameRenderResources) = 0;

    virtual void Shutdown() = 0;

    virtual void Resize(int width, int height) = 0;

    virtual void Execute(FramePassRenderContext &framePassRenderContext) = 0;
};

#endif
