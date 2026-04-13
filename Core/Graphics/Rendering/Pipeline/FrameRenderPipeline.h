#ifndef PINGPONG_FRAMERENDERPIPELINE_H
#define PINGPONG_FRAMERENDERPIPELINE_H

#include "Core/Graphics/Rendering/Pipeline/IRenderPass.h"

#include <memory>
#include <vector>

class FramePassRenderContext;
class FrameRenderResources;
class GraphicsDevice;
class FrameRenderResources;
class GraphicsDevice;

class FrameRenderPipeline final
{
public:
    FrameRenderPipeline() = default;

    FrameRenderPipeline(const FrameRenderPipeline &) = delete;

    FrameRenderPipeline &operator=(const FrameRenderPipeline &) = delete;

    FrameRenderPipeline(FrameRenderPipeline &&) = delete;

    FrameRenderPipeline &operator=(FrameRenderPipeline &&) = delete;

    ~FrameRenderPipeline();

    void Initialize(GraphicsDevice &graphicsDevice, FrameRenderResources &frameRenderResources);

    void Shutdown();

    void Resize(int width, int height);

    void ClearPasses();

    void AddPass(std::unique_ptr<IRenderPass> renderPass);

    void InsertPass(std::size_t index, std::unique_ptr<IRenderPass> renderPass);

    void ExecuteFrame(FramePassRenderContext &framePassRenderContext);

    [[nodiscard]] std::size_t GetPassCount() const noexcept;

private:
    std::vector<std::unique_ptr<IRenderPass>> m_passes{};
    bool m_initialized{false};
    GraphicsDevice *m_graphicsDevice{nullptr};
    FrameRenderResources *m_frameRenderResources{nullptr};
};

#endif
